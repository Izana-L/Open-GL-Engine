#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <Component.hpp>
#include <Transform_Component.hpp>

namespace open_gl_engine
{
    /// @brief Componente que representa una cámara en el motor.
    /// Almacena los parámetros de proyección (fov, aspect ratio, planos de recorte)
    /// y proporciona las matrices de vista y proyección necesarias para el renderizado.
    struct Camera_Component : Component
    {
        float fov;               ///< Campo de visión vertical en grados.
        float aspect_ratio;      ///< Relación de aspecto (ancho/alto).
        float near_plane;        ///< Distancia al plano de recorte cercano.
        float far_plane;         ///< Distancia al plano de recorte lejano.

        glm::vec3 world_up;      ///< Vector 'arriba' del mundo (por defecto Y+).

        bool is_active;          ///< Indica si esta cámara está activa para el renderizado.

        /// @brief Constructor por defecto. Inicializa valores estándar y asigna la entidad.
        /// @param my_entity_id Identificador de la entidad a la que pertenece este componente.
        Camera_Component(Id& my_entity_id)
            : fov(45.0f),
            aspect_ratio(16.0f / 9.0f),
            near_plane(0.1f),
            far_plane(100.0f),
            world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
            is_active(false)
        {
            entity_id = my_entity_id; // Asigna el ID de la entidad propietaria
        }

        /// @brief Constructor parametrizado. Permite configurar todos los valores de la cámara.
        /// @param my_entity_id Identificador de la entidad.
        /// @param fov_val Campo de visión vertical en grados.
        /// @param aspect_val Relación de aspecto.
        /// @param near_val Plano cercano.
        /// @param far_val Plano lejano.
        /// @param active Si la cámara debe estar activa por defecto.
        Camera_Component(Id& my_entity_id,
            float fov_val,
            float aspect_val,
            float near_val,
            float far_val,
            bool active = false)
            : fov(fov_val),
            aspect_ratio(aspect_val),
            near_plane(near_val),
            far_plane(far_val),
            world_up(glm::vec3(0.0f, 1.0f, 0.0f)),
            is_active(active)
        {
            entity_id = my_entity_id; // Asigna el ID de la entidad propietaria
        }

        /// @brief Calcula la matriz de vista a partir de un componente de transformación.
        /// @param transform Referencia al Transform_Component de la entidad.
        /// @return Matriz de vista (lookAt).
        glm::mat4 get_view_matrix(const Transform_Component& transform) const
        {
            // Obtiene la posición mundial y la dirección hacia adelante
            glm::vec3 position = transform.get_world_position();
            glm::vec3 forward = transform.getForward();
            // Construye la matriz lookAt: posición, punto al que mira, vector arriba
            return glm::lookAt(position, position + forward, world_up);
        }

        /// @brief Calcula la matriz de vista a partir de una matriz de modelo mundial de la cámara.
        /// @param camera_world_matrix Matriz de transformación mundial de la cámara.
        /// @return Matriz de vista (inversa de la matriz mundial).
        glm::mat4 get_view_matrix(const glm::mat4& camera_world_matrix) const
        {
            // La vista es la inversa de la transformación de la cámara
            return glm::inverse(camera_world_matrix);
        }

        /// @brief Calcula la matriz de proyección perspectiva.
        /// @return Matriz de proyección.
        glm::mat4 get_projection_matrix() const
        {
            // Convierte fov a radianes y crea la proyección perspectiva
            return glm::perspective(glm::radians(fov), aspect_ratio, near_plane, far_plane);
        }
    };
}