
// Este código es de dominio público
// angel.rodriguez@udit.es

#pragma once

#include <SDL3/SDL.h>
#include <string>
#include <utility>

namespace open_gl_engine
{

    class Window
    {
    public:

        struct OpenGL_Context_Settings
        {
            unsigned version_major = 3;
            unsigned version_minor = 3;
            bool     core_profile = true;
            unsigned depth_buffer_size = 24;
            unsigned stencil_buffer_size = 0;
            bool     enable_vsync = true;
        };

    private:

        SDL_Window* window_handle;
        SDL_GLContext opengl_context;
        friend class Input_Manager;
        int height, width;
    public:

        Window
        (
            const std::string& title,
            unsigned width,
            unsigned height,
            const OpenGL_Context_Settings& context_details
        )
            :
            Window(title.c_str(), width, height, context_details)
        {
        }

        Window
        (
            const char* title,
            unsigned width,
            unsigned height,
            const OpenGL_Context_Settings& context_details
        );

        ~Window();

    public:


        Window(const Window&) = delete;

        Window& operator = (const Window&) = delete;

        Window(Window&& other) noexcept
        {
            this->window_handle = std::exchange(other.window_handle, nullptr);
            this->opengl_context = std::exchange(other.opengl_context, nullptr);
        }

        Window& operator = (Window&& other) noexcept
        {
            this->window_handle = std::exchange(other.window_handle, nullptr);
            this->opengl_context = std::exchange(other.opengl_context, nullptr);
        }

    public:

        void swap_buffers();
		int get_width() const { return width; }
        int get_height() const { return height; }
        
        int getFramebufferWidth() const;
        int getFramebufferHeight() const;
        void set_title(const std::string& title);
    };

}
