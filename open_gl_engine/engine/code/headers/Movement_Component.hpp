#pragma once
#include <glm.hpp>
#include <Component.hpp>

namespace open_gl_engine
{
    struct Movement_Component : Component
    {
        glm::vec3 velocity;               ///< Velocidad lineal actual (unidades/segundo)
        glm::vec3 acceleration;           ///< Aceleración lineal (unidades/segundo²)
        glm::vec3 angular_velocity;       ///< Velocidad angular (radianes/segundo)
        glm::vec3 angular_acceleration;   ///< Aceleración angular (radianes/segundo²)

        float linear_damping;             ///< 0 = sin pérdida, 1 = se detiene en 1 s
        float angular_damping;            ///< 0 = sin pérdida
        float max_linear_speed;           ///< 0 = sin límite
        float max_angular_speed;          ///< 0 = sin límite

        /// @brief Constructor básico. Inicializa todos los valores a cero.
        /// @param _entity_id Identificador de la entidad propietaria.
        Movement_Component(Id& _entity_id)
            : velocity(glm::vec3(0.0f)), acceleration(glm::vec3(0.0f)),
            angular_velocity(glm::vec3(0.0f)), angular_acceleration(glm::vec3(0.0f)),
            linear_damping(0.0f), angular_damping(0.0f),
            max_linear_speed(0.0f), max_angular_speed(0.0f)
        {
            entity_id = _entity_id;
        }

        /// @brief Constructor con parámetros. Permite especificar cada vector individualmente.
        /// @param _entity_id Identificador de la entidad.
        /// @param vel Velocidad lineal inicial (por defecto cero).
        /// @param accel Aceleración lineal inicial (por defecto cero).
        /// @param ang_vel Velocidad angular inicial (por defecto cero, en radianes/segundo).
        /// @param ang_accel Aceleración angular inicial (por defecto cero, en radianes/segundo²).
        Movement_Component(Id& _entity_id,
            const glm::vec3& vel = glm::vec3(0.0f),
            const glm::vec3& accel = glm::vec3(0.0f),
            const glm::vec3& ang_vel = glm::vec3(0.0f),
            const glm::vec3& ang_accel = glm::vec3(0.0f))
            : velocity(vel), acceleration(accel),
            angular_velocity(ang_vel), angular_acceleration(ang_accel),
            linear_damping(0.0f), angular_damping(0.0f),
            max_linear_speed(0.0f), max_angular_speed(0.0f)
        {
            entity_id = _entity_id;
        }
    };
}