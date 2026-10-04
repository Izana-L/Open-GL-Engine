#pragma once

/// @brief Clase base abstracta para todas las tareas del motor.
/// Cada tarea representa una unidad de trabajo que se ejecuta cada frame
/// en el bucle principal de la Scene.
namespace open_gl_engine
{
    class Scene;

    class Task
    {
    protected:
        Scene* scene = nullptr; ///< Puntero a la escena propietaria de la tarea.

    public:
        /// @brief Método virtual puro que contiene la lógica de la tarea.
        /// Debe implementarse en las clases derivadas.
        virtual void execute() = 0;

        /// @brief Destructor virtual para permitir herencia polimórfica.
        virtual ~Task() = default;
    };
}