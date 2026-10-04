#include "Entity_Creator.hpp"
#include <Scene.hpp>
#include <Transform_Component.hpp>
#include <Mesh_Component.hpp>
#include <Model_Loader.hpp>
#include <Texture_Component.hpp>
#include <Camera_Component.hpp>
#include <Mesh_Directory.hpp>
#include <Texture_Manager.hpp>
#include <Skybox_Manager.hpp>
#include <Heightmap.hpp>
#include <cassert>

namespace open_gl_engine 
{
   
        // -----------------------------------------------------------------------------
        // Función auxiliar común para cualquier primitiva (evita duplicar código)
        // -----------------------------------------------------------------------------
        static Id create_primitive(Scene& scene,
            const Mesh* (*mesh_func)(),   // puntero a función miembro de Mesh_Directory
            const glm::vec3& position,
            Id parent_id,
            const std::string& texture_name)
        {
            Id entity_id = scene.add_entity();

            // Transform
            scene.add_component<Transform_Component>(entity_id);
            auto transform_opt = scene.get_component<Transform_Component>(entity_id);
            Transform_Component& trans = transform_opt->get();
            trans.set_position(position);
           

            if (parent_id != INVALID_ID)
            {
                auto parent_trans_opt = scene.get_component<Transform_Component>(parent_id);
                if (parent_trans_opt.has_value())
                    trans.set_parent(&parent_trans_opt->get());
            }

            // Mesh
            scene.add_component<Mesh_Component>(entity_id);
            auto mesh_opt = scene.get_component<Mesh_Component>(entity_id);
            mesh_opt->get().mesh = mesh_func();   // llama a create_cone_mesh, create_plane_mesh, etc.

            // Textura opcional
            if (!texture_name.empty())
            {
                scene.add_component<Texture_Component>(entity_id);
                auto tex_opt = scene.get_component<Texture_Component>(entity_id);
                tex_opt->get().texture = Texture_Manager::get().load_texture(texture_name);
            }

            return entity_id;
        }

        // -----------------------------------------------------------------------------
        // Funciones públicas (una por cada primitiva)
        // -----------------------------------------------------------------------------
        Id create_cone(Scene& scene, const glm::vec3& position, Id parent_id,
            const std::string& texture_name)
        {
            return create_primitive(scene, []() { return Mesh_Directory::get().create_cone_mesh(); },
                position, parent_id, texture_name);
        }

        Id create_plane(Scene& scene, const glm::vec3& position, Id parent_id,
            const std::string& texture_name)
        {
            return create_primitive(scene, []() { return Mesh_Directory::get().create_plane_mesh(); },
                position, parent_id, texture_name);
        }

        Id create_cylinder(Scene& scene, const glm::vec3& position, Id parent_id,
            const std::string& texture_name)
        {
            return create_primitive(scene, []() { return Mesh_Directory::get().create_cilinder_mesh(); },
                position, parent_id, texture_name);
        }

        Id create_sphere(Scene& scene, const glm::vec3& position, Id parent_id,
            const std::string& texture_name)
        {
            return create_primitive(scene, []() { return Mesh_Directory::get().create_sphere_mesh(); },
                position, parent_id, texture_name);
        }

        Id create_cube(Scene& scene, const glm::vec3& position, Id parent_id,
            const std::string& texture_name)
        {
            return create_primitive(scene, []() { return Mesh_Directory::get().create_cube_mesh(); },
                position, parent_id, texture_name);
        }

        // -----------------------------------------------------------------------------
        // Objeto importado (usa import_mesh y textura opcional)
        // -----------------------------------------------------------------------------

        Id create_object(Scene& scene,
            const std::string& mesh_name,
            const std::string& texture_name,
            const glm::vec3& position,
            Id parent_id)
        {
            // Cargar el modelo completo (puede tener varias mallas / nodos)
            Id root_id = texture_name.empty()
                ? load_model(scene, mesh_name)
                : load_model_with_texture(scene, mesh_name, texture_name);

            // Aplicar posición y parentesco al nodo raíz del modelo
            auto root_trans_opt = scene.get_component<Transform_Component>(root_id);
            assert(root_trans_opt.has_value());
            Transform_Component& root_trans = root_trans_opt->get();

            root_trans.set_position(position);

            if (parent_id != INVALID_ID)
            {
                auto parent_trans_opt = scene.get_component<Transform_Component>(parent_id);
                if (parent_trans_opt.has_value())
                    root_trans.set_parent(&parent_trans_opt->get());
            }

            return root_id;
        }
        // -----------------------------------------------------------------------------
        // Skybox (se apoya en Skybox_Manager)
        // -----------------------------------------------------------------------------
        void create_skybox(Scene& scene, const std::string& name, const std::vector<std::string>& face_texture_names)
        {
            // Cargar el skybox en el gestor (se le da un nombre fijo "default")
            Skybox_Manager::get().add_skybox(name, face_texture_names);
            Skybox_Manager::get().set_active(name);

           
        }
        // -----------------------------------------------------------------------------
        // Terreno
        // -----------------------------------------------------------------------------
        Id create_terrain(Scene& scene, const std::string& heightmap_texture_name,
            float height_scale, float size_scale)
        {
            Id terrain_id = scene.add_entity();

            // Transform (el terreno se coloca en el origen; puede moverse después)
            scene.add_component<Transform_Component>(terrain_id);
            auto trans_opt = scene.get_component<Transform_Component>(terrain_id);
            if (trans_opt)
                trans_opt->get().set_position(glm::vec3(0.0f, -7.0f, 0.0f));

            // Cargar heightmap desde imagen
            Heightmap heightmap = Heightmap::load_from_image(heightmap_texture_name, height_scale, size_scale);

            // Mesh del terreno
            scene.add_component<Mesh_Component>(terrain_id);
            auto mesh_opt = scene.get_component<Mesh_Component>(terrain_id);
            if (mesh_opt)
                mesh_opt->get().mesh = Mesh_Directory::get().create_terrain_mesh(heightmap);

            return terrain_id;
        }

        // -----------------------------------------------------------------------------
        // Cámara
        // -----------------------------------------------------------------------------
        Id create_camera(Scene& scene, const glm::vec3& position, Id parent_id)
        {
            Id camera_id = scene.add_entity();

            // Transform
            scene.add_component<Transform_Component>(camera_id);
            auto trans_opt = scene.get_component<Transform_Component>(camera_id);
            if (trans_opt)
            {
                trans_opt->get().set_position(position);
                if (parent_id != INVALID_ID)
                {
                    auto parent_opt = scene.get_component<Transform_Component>(parent_id);
                    if (parent_opt)
                        trans_opt->get().set_parent(&parent_opt->get());
                }
            }

            // Componente de cámara (activa por defecto)
            scene.add_component<Camera_Component>(camera_id);
            auto cam_opt = scene.get_component<Camera_Component>(camera_id);
            if (cam_opt)
                cam_opt->get().is_active = true;

            return camera_id;
        }
} 