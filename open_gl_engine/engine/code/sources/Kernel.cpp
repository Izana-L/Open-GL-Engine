#include <Kernel.hpp>
#include <ShaderCompiler.hpp>
#include <iostream>
#include <cassert>
#include <SDL3/SDL.h>
#include <glm.hpp>                          // vec3, vec4, ivec4, mat4
#include <gtc/matrix_transform.hpp>         // translate, rotate, scale, perspective
#include <gtc/type_ptr.hpp>  
#include <Window.hpp>
#include <Input_Manager.hpp>

namespace open_gl_engine
{
    /// @brief Constructor del núcleo. Inicializa el estado de OpenGL, compila el shader
    /// por defecto y guarda las localizaciones de los uniforms.
    Kernel::Kernel()
    {
        // Configuración global de renderizado
        glDisable(GL_CULL_FACE);
        glEnable(GL_CULL_FACE);          // Activa culling de caras traseras
        glEnable(GL_DEPTH_TEST);         // Test de profundidad
        glClearColor(.4f, .4f, .4f, 1.f); // Color de fondo gris

        // Habilitar blending para transparencias
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Cargar y compilar el shader por defecto
        program_id = load_and_compile_shaders("scene_default.vert", "scene_default.frag");

        glUseProgram(program_id);

        // Obtener localizaciones de los uniforms de transformación
        model_view_matrix_id = glGetUniformLocation(program_id, "model_view_matrix");
        projection_matrix_id = glGetUniformLocation(program_id, "projection_matrix");
        normal_matrix_id = glGetUniformLocation(program_id, "normal_matrix");

        // Tamaño inicial de la ventana
        unsigned viewport_width = 1024;
        unsigned viewport_height = 576;
        resize(viewport_width, viewport_height);
    }

    /// @brief Ajusta el viewport y recalcula la matriz de proyección perspectiva.
    void Kernel::resize(const int& width, const int& height)
    {
        // Crear matriz de proyección con FOV=20°, aspect ratio, planos cercano/lejano
        glm::mat4 projection_matrix = glm::perspective(20.f, GLfloat(width) / height, 1.f, 500.f);

        // Enviar matriz al shader
        glUniformMatrix4fv(projection_matrix_id, 1, GL_FALSE, glm::value_ptr(projection_matrix));

        // Actualizar viewport
        glViewport(0, 0, width, height);
    }

    /// @brief Bucle principal del motor. Gestiona eventos, delta time, entrada y ejecución de tareas.
    void Kernel::execute(Window& window)
    {
        last_time = std::chrono::steady_clock::now();
        exit = false;

        do
        {
            // Procesar todos los eventos SDL
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (event.type == SDL_EVENT_QUIT)
                {
                    stop(); // Salir al cerrar la ventana
                }
            }

            // Calcular delta time
            auto now = std::chrono::steady_clock::now();
            delta_time = std::chrono::duration<float>(now - last_time).count();
            last_time = now;
            float raw_fps = 1.0f / delta_time;
            frame_count++;

            if (frame_count % 60 == 0)   // actualizar cada 60 frames (aprox. 1 vez/segundo)
            {
                window.set_title("OpenGL Engine - FPS: " + std::to_string(static_cast<int>(raw_fps)));
            }
            // Actualizar estado de entrada (teclado/ratón)
            Input_Manager::get().update();

            // Ejecutar todas las tareas registradas
            for (auto& task : tasks) task->execute();

            // Intercambiar buffers (presentar frame)
            window.swap_buffers();

        } while (!exit);
    }
}