#pragma once

#include <array>
#include <memory>
#include <glm.hpp>
#include <SDL3/SDL.h>

namespace open_gl_engine
{
    class Window;

    /// @brief Singleton que encapsula la entrada de teclado y ratón mediante SDL3.
    /// Permite consultar el estado de teclas y botones, así como el movimiento relativo del ratón.
    /// Debe inicializarse una vez antes de su uso y actualizarse cada frame.
    class Input_Manager
    {
    private:
        SDL_Window* sdl_window; ///< Puntero a la ventana SDL asociada.

        static std::unique_ptr<Input_Manager> instance; ///< Instancia única del singleton.

        std::array<bool, SDL_SCANCODE_COUNT> key_current{};  ///< Estado actual de cada tecla (true = pulsada).
        std::array<bool, SDL_SCANCODE_COUNT> key_previous{}; ///< Estado del frame anterior para detectar pulsaciones.

        std::array<bool, 8> mouse_current{};  ///< Estado actual de los botones del ratón (índices 0-7).
        std::array<bool, 8> mouse_previous{}; ///< Estado anterior de los botones.

        glm::vec2 mouse_delta{ 0.0f }; ///< Desplazamiento del ratón en el último frame (píxeles).

        /// @brief Constructor privado. Configura el modo de ratón relativo y captura el estado inicial.
        /// @param window Puntero a la SDL_Window sobre la que se recoge la entrada.
        Input_Manager(SDL_Window* window);

        // No copiable / movible
        Input_Manager(const Input_Manager&) = delete;
        Input_Manager& operator=(const Input_Manager&) = delete;

    public:
        /// @brief Inicializa el singleton con la ventana proporcionada.
        /// @param window Referencia a la clase Window del motor.
        static void initialize(Window& window);

        /// @brief Obtiene la instancia única. Debe haberse inicializado antes.
        /// @return Referencia al Input_Manager.
        static Input_Manager& get();

        ~Input_Manager();

        /// @brief Actualiza el estado de teclas y ratón. Debe llamarse una vez por frame.
        void update();

        /// @brief Indica si una tecla está siendo mantenida pulsada.
        /// @param key Código SDL_Scancode de la tecla.
        /// @return true si la tecla está abajo en el frame actual.
        bool is_key_down(SDL_Scancode key) const;

        /// @brief Detecta si una tecla fue pulsada en este frame (flanco de subida).
        /// @param key Código SDL_Scancode.
        /// @return true si la tecla pasó de no pulsada a pulsada.
        bool is_key_pressed(SDL_Scancode key) const;

        /// @brief Devuelve el desplazamiento relativo del ratón desde el último frame.
        /// @return Vector2D con delta en X e Y (píxeles).
        glm::vec2 get_mouse_delta() const;

        /// @brief Indica si un botón del ratón está siendo mantenido pulsado.
        /// @param button Número de botón (SDL_BUTTON_LEFT = 1, etc.).
        /// @return true si el botón está abajo.
        bool is_mouse_button_down(Uint8 button) const;

        /// @brief Detecta si un botón del ratón fue pulsado en este frame.
        /// @param button Número de botón.
        /// @return true si el botón pasó a pulsado en este frame.
        bool is_mouse_button_pressed(Uint8 button) const;
    };
}