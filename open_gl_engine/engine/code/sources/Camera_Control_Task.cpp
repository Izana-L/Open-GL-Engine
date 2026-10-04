#define GLM_ENABLE_EXPERIMENTAL
#include "Camera_Control_Task.hpp"
#include <Scene.hpp>
#include <Transform_Component.hpp>
#include <Camera_Component.hpp>
#include <Input_Manager.hpp>
#include <gtc/quaternion.hpp>
#include <gtc/constants.hpp>
#include <gtx/vector_angle.hpp>
#include <cmath>

namespace open_gl_engine {

    Camera_Control_Task::Camera_Control_Task(Scene* _scene)
    {
        // Almacena la escena para acceder a componentes y al kernel
        scene = _scene;
    }

    void Camera_Control_Task::execute()
    {
        // 1. Obtener el transform de la cámara activa; si no hay, salir
        Transform_Component* transform = get_active_camera_transform();
        if (!transform) return;

        // 2. Obtener delta time para un movimiento independiente de la velocidad de frames
        float dt = scene->get_kernel().get_delta_time();
        if (dt <= 0.0f) dt = 1.0f / 60.0f;  // fallback si dt no es válido

        // 3. Aplicar movimiento de teclado
        handle_keyboard_movement(*transform, dt);

        // 4. Aplicar rotación de ratón
        handle_mouse_rotation(*transform);
    }

    Transform_Component* Camera_Control_Task::get_active_camera_transform() const {
        // Buscar entre todos los Camera_Component cuál está marcado como activo
        Camera_Component* active_camera = nullptr;
        auto cameras = scene->get_all_components_type<Camera_Component>();
        for (auto* cam : cameras) {
            if (cam->is_active) {
                active_camera = cam;
                break;
            }
        }
        // Si ninguna está marcada como activa, usar la primera cámara disponible
        if (!active_camera && !cameras.empty()) {
            active_camera = cameras.front();
        }
        if (!active_camera) return nullptr;

        // Obtener el transform asociado a la entidad de esa cámara
        Id cam_id = active_camera->get_entity();
        auto transform_opt = scene->get_component<Transform_Component>(cam_id);
        return transform_opt ? &transform_opt.value().get() : nullptr;
    }

    void Camera_Control_Task::handle_keyboard_movement(Transform_Component& transform, float dt)
    {
        glm::vec3 input_dir(0.0f);

        // Leer teclas de movimiento (WASD, espacio, shift) y construir vector de entrada
        if (Input_Manager::get().is_key_down(SDL_SCANCODE_W))     input_dir.z -= 1.0f;
        if (Input_Manager::get().is_key_down(SDL_SCANCODE_S))     input_dir.z += 1.0f;
        if (Input_Manager::get().is_key_down(SDL_SCANCODE_D))     input_dir.x += 1.0f;
        if (Input_Manager::get().is_key_down(SDL_SCANCODE_A))     input_dir.x -= 1.0f;
        if (Input_Manager::get().is_key_down(SDL_SCANCODE_SPACE)) input_dir.y += 1.0f;
        if (Input_Manager::get().is_key_down(SDL_SCANCODE_LSHIFT) ||
            Input_Manager::get().is_key_down(SDL_SCANCODE_RSHIFT))
            input_dir.y -= 1.0f;

        // Solo calcular desplazamiento si hay dirección de entrada
        if (glm::length(input_dir) > 0.0f) {
            input_dir = glm::normalize(input_dir);

            // Obtener ejes locales
            glm::vec3 forward = transform.getForward();
            glm::vec3 right = transform.getRight();

            // Proyectar en el plano horizontal para movimiento lateral y frontal
            glm::vec3 forward_h = glm::normalize(glm::vec3(forward.x, 0.0f, forward.z));
            glm::vec3 right_h = glm::normalize(glm::vec3(right.x, 0.0f, right.z));

            // Calcular vector desplazamiento final
            glm::vec3 displacement = (forward_h * (-input_dir.z) + right_h * input_dir.x +
                glm::vec3(0.0f, input_dir.y, 0.0f)) * move_speed * dt;
            // Aplicar movimiento
            transform.set_position(transform.get_local_position() + displacement);
        }
    }

    void Camera_Control_Task::handle_mouse_rotation(Transform_Component& transform)
    {
        // Obtener el delta del ratón desde el último frame
        glm::vec2 mouse_delta = Input_Manager::get().get_mouse_delta();
        if (mouse_delta.x == 0.0f && mouse_delta.y == 0.0f) return;

        // Convertir delta a ángulos de guiñada y cabeceo
        float yaw = -mouse_delta.x * mouse_sensitivity;
        float pitch = -mouse_delta.y * mouse_sensitivity;

        // Calcular el cabeceo actual para limitarlo
        glm::vec3 forward = transform.getForward();
        float current_pitch = glm::degrees(asinf(-forward.y)); // asume forward normalizado

        // Limitar el cabeceo al rango [-max_pitch, max_pitch]
        float desired_pitch = current_pitch + pitch;
        float clamped_pitch = glm::clamp(desired_pitch, -max_pitch, max_pitch);
        pitch = clamped_pitch - current_pitch;

        // Construir cuaterniones de rotación
        glm::quat current_rot = transform.get_rotation_quat();
        glm::quat yaw_quat = glm::angleAxis(glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));   // eje Y mundial
        glm::quat pitch_quat = glm::angleAxis(glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f)); // eje X local

        // Aplicar yaw * rotación actual * pitch, luego normalizar
        glm::quat new_rot = yaw_quat * current_rot * pitch_quat;
        transform.set_rotation_quat(glm::normalize(new_rot));
    }

} 