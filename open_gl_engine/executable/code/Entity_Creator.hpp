#pragma once

#pragma once
#include <string>
#include <glm.hpp>
#include <Id.hpp>

namespace open_gl_engine
{
    class Scene;

    
        /// @brief Crea una entidad con un cono.
        /// @param scene Escena donde crear la entidad.
        /// @param position Posición local (relativa al padre, o mundo si no hay padre).
        /// @param parent_id Id de la entidad padre (opcional, por defecto INVALID_ID = sin padre).
        /// @param texture_name Nombre de la textura a cargar (opcional, cadena vacía = sin textura).
        /// @param scale Escala local (por defecto 1).
        /// @return Id de la nueva entidad.
        Id create_cone(Scene& scene,
            const glm::vec3& position = glm::vec3(0.0f),
            Id parent_id = INVALID_ID,
            const std::string& texture_name = "");

        Id create_plane(Scene& scene,
            const glm::vec3& position = glm::vec3(0.0f),
            Id parent_id = INVALID_ID,
            const std::string& texture_name = "");

        Id create_cylinder(Scene& scene,
            const glm::vec3& position = glm::vec3(0.0f),
            Id parent_id = INVALID_ID,
            const std::string& texture_name = "");

        Id create_sphere(Scene& scene,
            const glm::vec3& position = glm::vec3(0.0f),
            Id parent_id = INVALID_ID,
            const std::string& texture_name = "");

        Id create_cube(Scene& scene,
            const glm::vec3& position = glm::vec3(0.0f),
            Id parent_id = INVALID_ID,
            const std::string& texture_name = "");

        /// @brief Crea una entidad con una malla importada (por nombre de archivo) y una textura opcional.
        /// @param mesh_name Nombre del archivo de malla (ej. "bunny.obj").
        /// @param texture_name Nombre del archivo de textura (opcional).
        /// @param position Posición inicial en el mundo.
        /// @param parent_id Id de la entidad padre (opcional).
        /// @param scale Escala local.
        /// @return Id de la nueva entidad.
        Id create_object(Scene& scene,
            const std::string& mesh_name,
            const std::string& texture_name = "",
            const glm::vec3& position = glm::vec3(0.0f),
            Id parent_id = INVALID_ID);

        void create_skybox(Scene& scene, const std::string& name, const std::vector<std::string>& face_texture_names);
        Id create_terrain(Scene& scene,const std::string& heightmap_texture_name,float height_scale = 10.0f,float size_scale = 50.0f);
		Id create_camera(Scene& scene, const glm::vec3& position = glm::vec3(0.0f), Id parent_id = INVALID_ID);
} // namespace open_gl_engine
