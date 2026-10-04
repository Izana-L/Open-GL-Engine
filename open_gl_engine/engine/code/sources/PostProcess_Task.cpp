#include <PostProcess_Task.hpp>
#include <Scene.hpp>
#include <Camera_Component.hpp>
#include <Transform_Component.hpp>
#include <Frame_Buffer_Manager.hpp>
#include <Mesh_Directory.hpp>
#include <Mesh.hpp>
#include <FboNames.hpp>
#include <ShaderCompiler.hpp>    // compile_shaders
#include <Window.hpp>
#include <cassert>

namespace open_gl_engine {

    PostProcess_Task::PostProcess_Task(Scene* _scene)
        : copy_program(0),
        copy_texture_loc(-1),
        quad_mesh(nullptr),
        frame_buffer_manager(Frame_Buffer_Manager::get())
    {
        // Asignar la escena (de la clase base Task)
        scene = _scene;

        // Obtener el quad del directorio de mallas (único para todos los postprocesos)
        quad_mesh = Mesh_Directory::get().create_quad_mesh();

        // Inicializar los efectos configurados en el motor
        init_effects();

        // Cargar el shader de copia final (quad.vert + copy.frag)
        copy_program = load_and_compile_shaders("quad.vert", "copy.frag");
        copy_texture_loc = glGetUniformLocation(copy_program, "screenTexture");
    }

    PostProcess_Task::~PostProcess_Task() {
        // Limpiar los programas de los efectos y el de copia
        for (auto& pass : effect_passes)
            glDeleteProgram(pass.program_id);
        glDeleteProgram(copy_program);
    }

    // Configura los passes de efectos: viñeta y FXAA.
    void PostProcess_Task::init_effects() {
        // Efecto de viñeta
        GLuint vignette_prog = load_and_compile_shaders("quad.vert", "vignette.frag");
        EffectPass vignette;
        vignette.program_id = vignette_prog;
        vignette.setup_uniforms = [this, vignette_prog]() {
            GLint loc = glGetUniformLocation(vignette_prog, "vignette_intensity");
            glUniform1f(loc, 0.5f);  // intensidad fija (luego podría ser ajustable)
            };
        effect_passes.push_back(vignette);

        // Efecto FXAA
        GLuint fxaa_prog = load_and_compile_shaders("quad.vert", "FXAA.frag");
        EffectPass fxaa_pass;
        fxaa_pass.program_id = fxaa_prog;
        fxaa_pass.setup_uniforms = [this, fxaa_prog]() {
            // Resolución inversa para el shader FXAA
            glm::vec2 fxaa_inv_resolution(1.0f / 1024.0f, 1.0f / 576.0f);
            fxaa_inv_resolution = glm::vec2(1.0f / scene->get_window().get_width(),
                1.0f / scene->get_window().get_height());
            GLint loc = glGetUniformLocation(fxaa_prog, "inv_resolution");
            glUniform2f(loc, fxaa_inv_resolution.x, fxaa_inv_resolution.y);
            };
        effect_passes.push_back(fxaa_pass);

        // Activar el flag de postprocesado en la escena
        if (!effect_passes.empty()) {
            scene->set_post_processing_active(true);
        }
    }

    // Ejecución por frame: obtiene el FBO de escena, aplica efectos y copia a pantalla.
    void PostProcess_Task::execute() {
        if (effect_passes.empty()) return;  // Nada que hacer

        Frame_Buffer* scene_fbo = frame_buffer_manager.get_framebuffer(FboNames::Scene);
        if (!scene_fbo) return;

        int width = scene_fbo->width;
        int height = scene_fbo->height;

        // Asegurar que existen FBOs Ping/Pong y están al tamaño adecuado
        ensure_ping_pong(width, height);

        // La entrada inicial es el framebuffer de la escena, la salida inicial es Ping
        Frame_Buffer* input_fbo = scene_fbo;
        Frame_Buffer* output_fbo = frame_buffer_manager.get_framebuffer(FboNames::Ping);

        // Ejecutar los efectos en cadena
        apply_effects(input_fbo, output_fbo, width, height);

        // El último efecto dejó el resultado en output_fbo; ahora copiar a pantalla
        final_copy(output_fbo);
    }

    // Encadena los passes. Va alternando entre Ping y Pong.
    void PostProcess_Task::apply_effects(Frame_Buffer* input_fbo, Frame_Buffer* output_fbo, int width, int height) {
        bool use_ping = true; // Para controlar cuál es el siguiente FBO de salida

        for (size_t i = 0; i < effect_passes.size(); ++i) {
            auto& pass = effect_passes[i];
            assert(pass.program_id != 0);

            // Usar el shader del efecto actual
            glUseProgram(pass.program_id);

            // Enlazar la textura del FBO de entrada al uniform 'screenTexture'
            input_fbo->bind_color_texture(0);
            glUniform1i(glGetUniformLocation(pass.program_id, "screenTexture"), 0);

            // Configurar uniforms personalizados del efecto
            pass.setup_uniforms();

            // Renderizar al FBO de salida
            output_fbo->bind();
            glViewport(0, 0, width, height);
            draw_quad();
            output_fbo->unbind();

            // Si no es el último paso, intercambiar entrada y salida para el siguiente
            if (i < effect_passes.size() - 1) {
                std::swap(input_fbo, output_fbo);
                // Alternar entre Ping y Pong para evitar leer y escribir en el mismo FBO
                if (use_ping) {
                    output_fbo = frame_buffer_manager.get_framebuffer(FboNames::Pong);
                    use_ping = false;
                }
                else {
                    output_fbo = frame_buffer_manager.get_framebuffer(FboNames::Ping);
                    use_ping = true;
                }
            }
        }
    }

    // Copia el contenido del último FBO a la pantalla (framebuffer 0) usando el shader de copia.
    void PostProcess_Task::final_copy(Frame_Buffer* final_fbo) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);          // Volver al framebuffer por defecto
        glViewport(0, 0, final_fbo->width, final_fbo->height);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(copy_program);
        final_fbo->bind_color_texture(0);
        glUniform1i(copy_texture_loc, 0);              // La textura del FBO procesado
        draw_quad();

        // Restaurar el programa de shader de la escena para que las siguientes tareas
        // de renderizado tengan el programa correcto
        glUseProgram(scene->get_kernel().get_id_program());
    }

    // Crea Ping y Pong si no existen, y los redimensiona al tamaño dado.
    void PostProcess_Task::ensure_ping_pong(int width, int height) {
        auto& fbo_mgr = Frame_Buffer_Manager::get();
        if (!fbo_mgr.get_framebuffer(FboNames::Ping))
            fbo_mgr.create_framebuffer(FboNames::Ping, width, height);
        if (!fbo_mgr.get_framebuffer(FboNames::Pong))
            fbo_mgr.create_framebuffer(FboNames::Pong, width, height);
        fbo_mgr.resize_all(width, height);
    }

    // Dibuja el quad a pantalla completa. Desactiva temporalmente depth test y face culling.
    void PostProcess_Task::draw_quad() {
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);

        glBindVertexArray(quad_mesh->vao_id);
        glDrawElements(GL_TRIANGLES, quad_mesh->index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        GLenum err = glGetError();
        assert(err == GL_NO_ERROR && "Error OpenGL tras draw_quad");

        glEnable(GL_DEPTH_TEST);   // Reestablecer estado por defecto
        glEnable(GL_CULL_FACE);
    }

} 