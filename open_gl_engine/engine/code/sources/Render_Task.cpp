#include <Scene.hpp>
#include <Render_Task.hpp>
#include <Mesh_Component.hpp>
#include <Mesh.hpp>
#include <Transform_Component.hpp>
#include <Texture_Manager.hpp>
#include <Texture_Component.hpp>
#include <Camera_Component.hpp>
#include <ComponentBits.hpp> 
#include <algorithm> 
#include <gtc/type_ptr.hpp> 
#include <cassert>
#include <Skybox_Manager.hpp>
#include <FboNames.hpp>               
#include <Frame_Buffer_Manager.hpp> 
#include <Window.hpp>

namespace open_gl_engine
{
    // Constructor: copia el kernel de la escena y pre-reserva vectores.
    Render_Task::Render_Task(Scene* my_scene)
        : view_matrix(glm::mat4(1.0f)),
        projection_matrix(glm::mat4(1.0f))
    {
        scene = my_scene;
        // Copia del kernel para acceder a sus uniform locations sin modificar el original.
        kernel = std::make_unique<Kernel>(my_scene->get_kernel());
        opaque_meshes.reserve(400);
        translucent_meshes.reserve(400);
    }

    // Configura el material: color base y enlace de textura.
    void Render_Task::configure_material(unsigned int program_id, const glm::vec3& color, const Texture* texture)
    {
        GLint material_color = glGetUniformLocation(program_id, "material_color");
        glUniform3f(material_color, color[0], color[1], color[2]);

        GLint sampler_loc = glGetUniformLocation(program_id, "sampler2d");
        if (sampler_loc != -1)
        {
            glUniform1i(sampler_loc, 0);   // unidad de textura 0
            if (texture)
                texture->bind(0);
            else
                Texture::unbind(0);
        }
    }

    void Render_Task::configure_General_light()
    {
        unsigned int program_id = kernel->get_id_program();

        GLint loc = glGetUniformLocation(program_id, "ambient_intensity");
        glUniform1f(loc, general_light.ambient_intensity);

        loc = glGetUniformLocation(program_id, "diffuse_intensity");
        glUniform1f(loc, general_light.diffuse_intensity);

        loc = glGetUniformLocation(program_id, "specular_intensity");
        glUniform1f(loc, general_light.specular_intensity);

        loc = glGetUniformLocation(program_id, "shininess");
        glUniform1f(loc, general_light.shininess);
    }

    void Render_Task::configure_Point_light()
    {
        unsigned int program_id = kernel->get_id_program();

        glm::vec4 world_pos(point_light.world_position, 1.0f);
        glm::vec3 view_pos = glm::vec3(view_matrix * world_pos);

        GLint loc = glGetUniformLocation(program_id, "point_light.light_position");
        glUniform3fv(loc, 1, glm::value_ptr(view_pos));

        loc = glGetUniformLocation(program_id, "point_light.light_color");
        glUniform3fv(loc, 1, glm::value_ptr(point_light.color));

        loc = glGetUniformLocation(program_id, "point_light.light_intensity");
        glUniform1f(loc, point_light.intensity);
    }

    void Render_Task::configure_Directional_light()
    {
        unsigned int program_id = kernel->get_id_program();

        glm::vec4 world_dir(directional_light.world_direction, 0.0f);
        glm::vec3 view_dir = glm::normalize(glm::vec3(view_matrix * world_dir));

        GLint loc = glGetUniformLocation(program_id, "directional_light.light_direction");
        glUniform3fv(loc, 1, glm::value_ptr(view_dir));

        loc = glGetUniformLocation(program_id, "directional_light.light_color");
        glUniform3fv(loc, 1, glm::value_ptr(directional_light.color));

        loc = glGetUniformLocation(program_id, "directional_light.light_intensity");
        glUniform1f(loc, directional_light.intensity);

        loc = glGetUniformLocation(program_id, "directional_light_enabled");
        glUniform1i(loc, directional_light.enabled ? 1 : 0);
    }

    // Dibuja una malla con su transformación.
    void Render_Task::draw_mesh(const Mesh_Component* mesh_component, const Texture* texture)
    {
        Id entityId = mesh_component->get_entity();
        auto transOpt = scene->get_component<Transform_Component>(entityId);
        if (!transOpt) return;
        Transform_Component& transform = transOpt->get();

        // Aplicar material
        configure_material(kernel->get_id_program(), glm::vec3(1.0f), texture);

        // Calcular matrices y enviar al shader
        glm::mat4 model_view = view_matrix * transform.get_transform_matrix();
        glm::mat3 normal = glm::transpose(glm::inverse(model_view));
        glm::mat4 normal4 = glm::mat4(normal);
        glUniformMatrix4fv(kernel->model_view_matrix_id, 1, GL_FALSE, glm::value_ptr(model_view));
        glUniformMatrix4fv(kernel->normal_matrix_id, 1, GL_FALSE, glm::value_ptr(normal4));

        // Dibujar
        glBindVertexArray(mesh_component->mesh->vao_id);
        glDrawElements(GL_TRIANGLES, mesh_component->mesh->index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

    // Renderiza opacos (escritura de profundidad habilitada).
    void Render_Task::render_opaque_objects(const std::vector<Mesh_Component*>& meshes, const Texture* default_tex)
    {
        glDepthMask(GL_TRUE);
        for (const auto* mesh_component : meshes)
        {
            const Texture* tex = default_tex;
            uint32_t mask = scene->get_entity_mask(mesh_component->get_entity());
            if (mask & CB_Texture)
            {
                auto tc = scene->get_component<Texture_Component>(mesh_component->get_entity());
                if (tc && tc->get().texture)
                    tex = tc->get().texture;
            }
            draw_mesh(mesh_component, tex);
        }
    }

    // Renderiza translúcidos ordenados de más lejano a más cercano (sin escritura de profundidad).
    void Render_Task::render_translucent_objects(const std::vector<Mesh_Component*>& meshes, const Texture* default_tex, const glm::vec3& camera_position)
    {
        if (meshes.empty()) return;

        std::vector<Mesh_Component*> sorted = meshes;
        // Ordenar por distancia a la cámara (descendente)
        std::sort(sorted.begin(), sorted.end(), [&](Mesh_Component* a, Mesh_Component* b) {
            auto tranform_a = scene->get_component<Transform_Component>(a->get_entity());
            auto transform_b = scene->get_component<Transform_Component>(b->get_entity());
            float distance_a = tranform_a ? glm::distance(tranform_a->get().get_world_position(), camera_position) : 0.0f;
            float distance_b = transform_b ? glm::distance(transform_b->get().get_world_position(), camera_position) : 0.0f;
            return distance_a > distance_b; // lejos primero
            });

        glDepthMask(GL_FALSE); // no escribir profundidad
        for (const auto* mesh_component : sorted)
        {
            const Texture* tex = default_tex;
            uint32_t mask = scene->get_entity_mask(mesh_component->get_entity());
            if (mask & CB_Texture)
            {
                auto tc = scene->get_component<Texture_Component>(mesh_component->get_entity());
                if (tc && tc->get().texture)
                    tex = tc->get().texture;
            }
            draw_mesh(mesh_component, tex);
        }
        glDepthMask(GL_TRUE);
    }

    // Ejecución principal del renderizado de un frame.
    void Render_Task::execute()
    {
        // 1. Obtener cámara activa (o la primera si ninguna marcada)
        auto cameras = scene->get_all_components_type<Camera_Component>();
        assert(!cameras.empty() && "Error: Empty cameras list");
        Camera_Component* camera = nullptr;
        for (auto* cam : cameras)
        {
            if (cam->is_active) { camera = cam; break; }
        }
        if (!camera) camera = cameras.front();

        auto wrapped_camera_transform = scene->get_component<Transform_Component>(camera->get_entity());
        Transform_Component& camera_transform = wrapped_camera_transform.value().get();
        glm::vec3 camara_position = camera_transform.get_world_position();

        // 2. Calcular matrices de vista y proyección
        view_matrix = camera->get_view_matrix(camera_transform.get_transform_matrix());
        projection_matrix = camera->get_projection_matrix();

        // 3. Enviar proyección al shader y configurar iluminación
        glUniformMatrix4fv(kernel->projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));
		configure_General_light();
        configure_Point_light();
        configure_Directional_light();
        // 4. Configurar el destino de renderizado (FBO de escena si postprocesado activo, si no, backbuffer)
        Frame_Buffer* scene_fbo = nullptr;
        int window_width = scene->get_window().get_width();
        int window_height = scene->get_window().get_height();

        if (scene->is_post_processing_active())
        {
            scene_fbo = Frame_Buffer_Manager::get().get_framebuffer(FboNames::Scene);
            if (!scene_fbo) {
                scene_fbo = Frame_Buffer_Manager::get().create_framebuffer(FboNames::Scene, window_width, window_height);
            }
            scene_fbo->bind();
            glViewport(0, 0, scene_fbo->width, scene_fbo->height);
        }
        else
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window_width, window_height);
        }

        // 5. Limpiar buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 6. Renderizar skybox si está activo
        if (auto* sky = Skybox_Manager::get().get_active())
        {
            glUseProgram(sky->get_program());
            sky->render(view_matrix, projection_matrix);
            glUseProgram(kernel->get_id_program()); // volver al shader de escena
        }

        // 7. Clasificar mallas en opacas y translúcidas
        opaque_meshes.clear();
        translucent_meshes.clear();

        std::vector<Mesh_Component*> meshes = scene->get_all_components_type<Mesh_Component>();
        assert(!meshes.empty() && "Error: Not meshes created");
        const Texture* default_texture = Texture_Manager::get().get_white_texture();

        for (auto* mesh_Component : meshes)
        {
            Id id = mesh_Component->get_entity();
            uint32_t mask = scene->get_entity_mask(id);
            if ((mask & (CB_Transform | CB_Mesh)) != (CB_Transform | CB_Mesh))
                continue; // necesita transform y malla

            const Texture* texture = default_texture;
            if (mask & CB_Texture)
            {
                auto texture_component = scene->get_component<Texture_Component>(id);
                if (texture_component && texture_component->get().texture)
                    texture = texture_component->get().texture;
            }

            // Decidir por opacidad de la textura
            if (texture && texture->has_alpha_less_than_one)
                translucent_meshes.push_back(mesh_Component);
            else
                opaque_meshes.push_back(mesh_Component);
        }

        // 8. Renderizar opacos y después translúcidos
        render_opaque_objects(opaque_meshes, default_texture);
        render_translucent_objects(translucent_meshes, default_texture, camara_position);

        // 9. Desvincular FBO si se usó
        if (scene_fbo) {
            scene_fbo->unbind();
        }
    }

} 