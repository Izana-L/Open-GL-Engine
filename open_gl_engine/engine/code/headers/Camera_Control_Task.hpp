#pragma once

#include <glm.hpp>
#include <Task.hpp>

namespace open_gl_engine
{
    class Scene;
    struct Transform_Component;

    /// @brief Tarea que controla en tiempo real la cámara activa.
    /// Lee entrada de teclado y ratón para mover y rotar la cámara cada frame.
    class Camera_Control_Task : public Task
    {
    public:
        /// @brief Constructor. Guarda la referencia a la escena.
        /// @param scene Puntero a la Scene propietaria de la tarea.
        Camera_Control_Task(Scene* scene);

        /// @brief Ejecuta la lógica de control cada frame.
        void execute() override;

        float move_speed = 5.0f;        ///< Velocidad de desplazamiento (unidades/segundo).
        float mouse_sensitivity = 0.1f; ///< Sensibilidad del ratón (grados/píxel).
        float max_pitch = 89.0f;        ///< Ángulo máximo de cabeceo (grados).

    private:
        /// @brief Obtiene el Transform_Component de la cámara marcada como activa.
        /// @return Puntero al Transform_Component o nullptr si no se encuentra.
        Transform_Component* get_active_camera_transform() const;

        /// @brief Procesa el movimiento por teclado y actualiza la posición.
        /// @param transform Referencia al transform de la cámara activa.
        /// @param dt Delta time en segundos.
        void handle_keyboard_movement(Transform_Component& transform, float dt);

        /// @brief Procesa la rotación por ratón y actualiza la orientación.
        /// @param transform Referencia al transform de la cámara activa.
        void handle_mouse_rotation(Transform_Component& transform);
    };
}