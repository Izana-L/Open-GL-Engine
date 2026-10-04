#include "Input_Manager.hpp"
#include <cassert>
#include <Window.hpp>

namespace open_gl_engine {

    std::unique_ptr<Input_Manager> Input_Manager::instance = nullptr;

    // Inicializa el singleton con la ventana; solo si aún no existe.
    void Input_Manager::initialize(Window& window)
    {
        if (instance) return; // Ya inicializado
        // Accede al miembro privado window_handle de Window (amistad implícita o friend)
        instance.reset(new Input_Manager(window.window_handle));
    }

    // Obtiene la instancia. Requiere que se haya llamado a initialize.
    Input_Manager& Input_Manager::get() {
        assert(instance && "Input_Manager no inicializado. Llama a Input_Manager::initialize() primero.");
        return *instance;
    }

    // Constructor: activa el modo de ratón relativo (cursor oculto, movimiento ilimitado)
    // y captura el estado inicial de teclas y botones para evitar falsos positivos.
    Input_Manager::Input_Manager(SDL_Window* _sdl_window) : sdl_window(_sdl_window) {
        SDL_SetWindowRelativeMouseMode(sdl_window, true);

        // Estado inicial de teclas
        int numkeys;
        const bool* state = SDL_GetKeyboardState(&numkeys);
        int count = numkeys < SDL_SCANCODE_COUNT ? numkeys : SDL_SCANCODE_COUNT;
        std::copy(state, state + count, key_current.begin());
        key_previous = key_current; // Sin cambios previos

        // Estado inicial de botones del ratón
        Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
        for (int i = 0; i < 8; ++i) {
            mouse_current[i] = (buttons & SDL_BUTTON_MASK(i + 1)) != 0;
            mouse_previous[i] = mouse_current[i];
        }
    }

    // Destructor: restaura el modo de ratón normal.
    Input_Manager::~Input_Manager() {
        SDL_SetWindowRelativeMouseMode(sdl_window, false);
    }

    // Actualización por frame: guarda estados anteriores, lee los nuevos y calcula delta del ratón.
    void Input_Manager::update() {
        // Teclado: copia el estado actual a previous y actualiza current
        key_previous = key_current;
        int numkeys;
        const bool* state = SDL_GetKeyboardState(&numkeys);
        int count = numkeys < SDL_SCANCODE_COUNT ? numkeys : SDL_SCANCODE_COUNT;
        std::copy(state, state + count, key_current.begin());

        // Ratón: obtiene movimiento relativo y lo almacena
        float delta_x, delta_y;
        SDL_GetRelativeMouseState(&delta_x, &delta_y);
        mouse_delta = glm::vec2(delta_x, delta_y);

        // Botones: actualiza previous y current
        mouse_previous = mouse_current;
        Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
        for (int i = 0; i < 8; ++i) {
            mouse_current[i] = (buttons & SDL_BUTTON_MASK(i + 1)) != 0;
        }
    }

    // Tecla mantenida: devuelve el estado actual.
    bool Input_Manager::is_key_down(SDL_Scancode key) const {
        return key < SDL_SCANCODE_COUNT ? key_current[key] : false;
    }

    // Tecla recién pulsada: true si ahora está abajo y antes no.
    bool Input_Manager::is_key_pressed(SDL_Scancode key) const {
        return key < SDL_SCANCODE_COUNT ? (key_current[key] && !key_previous[key]) : false;
    }

    // Delta del ratón acumulado en este frame.
    glm::vec2 Input_Manager::get_mouse_delta() const {
        return mouse_delta;
    }

    // Botón del ratón mantenido. Los botones SDL empiezan en 1, arrays internos en 0.
    bool Input_Manager::is_mouse_button_down(Uint8 button) const {
        if (button < 1 || button > 8) return false;
        return mouse_current[button - 1];
    }

    // Botón recién pulsado: flanco de subida.
    bool Input_Manager::is_mouse_button_pressed(Uint8 button) const {
        if (button < 1 || button > 8) return false;
        return mouse_current[button - 1] && !mouse_previous[button - 1];
    }

} 