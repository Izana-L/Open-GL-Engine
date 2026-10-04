#include <Transform_Update_Task.hpp>
#include <Scene.hpp>
#include <Transform_Component.hpp>
#include <Movement_Component.hpp>
#include <ComponentBits.hpp>

namespace open_gl_engine
{
    Transform_Update_Task::Transform_Update_Task(Scene* _scene)
    {
		scene = _scene;
    }

    void Transform_Update_Task::execute()
    {
        float dt = scene->get_kernel().get_delta_time();
        if (dt <= 0.0f) dt = 1.0f / 60.0f;

        auto transforms = scene->get_all_components_type<Transform_Component>();
        for (auto* transform : transforms)
        {
            Id entityId = transform->get_entity();
            uint32_t mask = scene->get_entity_mask(entityId);
            if (!(mask & CB_Movement)) continue;

            auto movementOpt = scene->get_component<Movement_Component>(entityId);
            if (!movementOpt) continue;

            Movement_Component& mov = movementOpt.value().get();

            // --- 1. Actualizar aceleraciones externas (opcional) ---
            // (Aquí podrías añadir más componentes que aporten aceleración)

            // --- 2. Integrar velocidades con aceleración ---
            mov.velocity += mov.acceleration * dt;
            mov.angular_velocity += mov.angular_acceleration * dt;

            // --- 3. Aplicar damping (resistencia) ---
            float linear_scale = 1.0f - mov.linear_damping * dt;
            if (linear_scale < 0.0f) linear_scale = 0.0f;
            mov.velocity *= linear_scale;

            float angular_scale = 1.0f - mov.angular_damping * dt;
            if (angular_scale < 0.0f) angular_scale = 0.0f;
            mov.angular_velocity *= angular_scale;

            // --- 4. Limitar velocidades máximas ---
            if (mov.max_linear_speed > 0.0f)
            {
                float lenth = glm::length(mov.velocity);
                if (lenth > mov.max_linear_speed)
                    mov.velocity *= mov.max_linear_speed / lenth;
            }
            if (mov.max_angular_speed > 0.0f)
            {
                float len = glm::length(mov.angular_velocity);
                if (len > mov.max_angular_speed)
                    mov.angular_velocity *= mov.max_angular_speed / len;
            }

            // --- 5. Actualizar posición ---
            glm::vec3 pos = transform->get_local_position();
            pos += mov.velocity * dt;
            transform->set_position(pos);

            // --- 6. Actualizar rotación (velocidad angular en rad/s) ---
            float angle = glm::length(mov.angular_velocity) * dt;
            if (angle > 0.0f)
            {
                glm::vec3 axis = glm::normalize(mov.angular_velocity);
                glm::quat deltaRot = glm::angleAxis(angle, axis);
                glm::quat currentRot = transform->get_rotation_quat();
                glm::quat newRot = currentRot * deltaRot;
                newRot = glm::normalize(newRot);
                transform->set_rotation_quat(newRot);
            }
        }
    }
}