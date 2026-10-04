#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <glm.hpp>
#include <glad/gl.h>
#include <Task.hpp>

namespace open_gl_engine {

    struct Mesh;
    class Frame_Buffer;
    class Frame_Buffer_Manager;

    /// @brief Tarea de postprocesado que aplica una cadena de efectos sobre la imagen de la escena.
    /// Utiliza un sistema de ping-pong entre framebuffers para encadenar múltiples pasos (viñeta, FXAA...)
    /// y finalmente copia el resultado a la pantalla.
    class PostProcess_Task : public Task
    {
    private:
        /// @brief Estructura que representa un paso de efecto: programa y lambda para configurar uniforms.
        struct EffectPass
        {
            GLuint program_id;                     ///< ID del programa GL del efecto.
            std::function<void()> setup_uniforms; ///< Función que establece los uniforms necesarios antes del draw.
        };

        std::vector<EffectPass> effect_passes;     ///< Lista de pasos de postprocesado.

        const Mesh* quad_mesh;                     ///< Malla del quad que ocupa toda la pantalla.
        Frame_Buffer_Manager& frame_buffer_manager; ///< Referencia al gestor de framebuffers (singleton).

        // Shader de copia final a la pantalla
        GLuint copy_program;        ///< Programa GL para la copia final (quad.vert + copy.frag).
        GLint  copy_texture_loc;    ///< Localización del uniform 'screenTexture' en el shader de copia.

    public:
        /// @brief Constructor. Recibe la escena para obtener la ventana y registrar la tarea.
        /// @param scene Puntero a la Scene propietaria.
        explicit PostProcess_Task(Scene* scene);

        ~PostProcess_Task();

        /// @brief Ejecuta la cadena de postprocesado cada frame.
        void execute() override;

    private:
        /// @brief Aplica todos los efectos en secuencia usando ping-pong FBOs.
        /// @param input_fbo Framebuffer de entrada (normalmente el de escena).
        /// @param output_fbo Framebuffer de salida inicial (Ping).
        /// @param width Ancho en píxeles.
        /// @param height Alto en píxeles.
        void apply_effects(Frame_Buffer* input_fbo, Frame_Buffer* output_fbo, int width, int height);

        /// @brief Copia el resultado final del último FBO a la pantalla (framebuffer por defecto).
        /// @param final_fbo Framebuffer que contiene la imagen procesada.
        void final_copy(Frame_Buffer* final_fbo);

        /// @brief Inicializa y almacena los passes de efectos (viñeta, FXAA, etc.).
        void init_effects();

        /// @brief Crea los FBOs Ping/Pong si no existen y los redimensiona.
        /// @param width Ancho actual.
        /// @param height Alto actual.
        void ensure_ping_pong(int width, int height);

        /// @brief Dibuja el quad a pantalla completa con el VAO ya configurado.
        void draw_quad();
    };
}