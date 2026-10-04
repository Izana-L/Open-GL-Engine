#pragma once

#include <vector>
#include <memory>
#include <chrono>
#include <glm.hpp>  
#include <glad/gl.h>
#include <Task.hpp>

namespace open_gl_engine
{
    class Input_Manager;
    class Window;

    /// @brief Núcleo del motor. Controla el bucle principal, las tareas, el tiempo delta
    /// y el shader por defecto para la escena.
    class Kernel
    {
        GLint program_id;                           ///< ID del programa de shader por defecto.

        std::vector<std::shared_ptr<Task>> tasks;   ///< Tareas que se ejecutan cada frame.

        bool exit;                                  ///< Bandera de salida del bucle principal.

        float delta_time = 0.0f;                    ///< Duración del último frame (segundos).
        int   frame_count = 0;
        std::chrono::steady_clock::time_point last_time; ///< Instante del frame anterior.

    public:
        GLint model_view_matrix_id;  ///< Localización del uniform model_view_matrix.
        GLint projection_matrix_id;  ///< Localización del uniform projection_matrix.
        GLint normal_matrix_id;      ///< Localización del uniform normal_matrix.

        /// @brief Constructor. Configura estados OpenGL, compila el shader por defecto y
        /// obtiene las localizaciones de los uniforms principales.
        Kernel();

        /// @brief Destructor por defecto.
        ~Kernel() {}

        /// @brief Añade una tarea al principio de la lista de tareas del bucle.
        /// @param new_task Puntero compartido a la tarea.
        void add_task(std::shared_ptr<Task> new_task)
        {
            tasks.insert(tasks.begin(), new_task);
        }

        /// @brief Solicita la terminación del bucle principal.
        void stop()
        {
            exit = true;
        }

        /// @brief Ejecuta el bucle principal del motor.
        /// @param window Referencia a la ventana donde se renderiza.
        void execute(Window&);

        /// @brief Redimensiona el viewport y actualiza la matriz de proyección.
        /// @param width Nuevo ancho en píxeles.
        /// @param height Nuevo alto en píxeles.
        void resize(const int& width, const int& height);

        /// @brief Devuelve el tiempo delta del último frame.
        float get_delta_time() const { return delta_time; }

        /// @brief Devuelve el ID del programa de shader por defecto.
        GLint get_id_program() { return program_id; }
    };
}