#pragma once

#include <Task.hpp>

namespace open_gl_engine
{
    class Scene;

    /// @brief Tarea que actualiza las transformaciones aplicando física básica (velocidad/aceleración).
    /// Recorre todas las entidades que poseen Transform_Component y Movement_Component,
    /// integrando posición y rotación en función del delta time.
    class Transform_Update_Task : public Task
    {
    public:
        /// @brief Constructor. Guarda la referencia a la escena.
        /// @param scene Puntero a la Scene propietaria.
        explicit Transform_Update_Task(Scene* scene);

        /// @brief Ejecuta la actualización física de los transforms cada frame.
        void execute() override;
    };
}