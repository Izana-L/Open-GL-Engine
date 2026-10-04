#pragma once

#include <string>
#include <cassert>
#include <Id.hpp>

namespace open_gl_engine
{
    /// @brief Envoltura RAII para un framebuffer de OpenGL.
    /// Gestiona un FBO con una textura de color adjunta y, opcionalmente,
    /// un renderbuffer de profundidad/stencil. Permite redimensionar y enlazar.
    class Frame_Buffer
    {
    public:
        unsigned int framebuffer_object_id = 0; ///< ID del framebuffer de OpenGL.
        unsigned int texture_id = 0;            ///< Textura de color asociada al FBO.
        unsigned int renderbuffer_object_id = 0;///< Renderbuffer de profundidad/stencil (0 si no se usa).

        int width = 0;      ///< Ancho actual del framebuffer en píxeles.
        int height = 0;     ///< Alto actual del framebuffer en píxeles.

        std::string name;   ///< Nombre descriptivo para depuración.

        /// @brief Constructor por defecto. Crea un framebuffer vacío.
        Frame_Buffer() = default;

        /// @brief Constructor principal. Crea el FBO y sus adjuntos.
        /// @param name Nombre identificativo.
        /// @param w Ancho en píxeles.
        /// @param h Alto en píxeles.
        /// @param with_depth Si true, añade renderbuffer de profundidad/stencil.
        Frame_Buffer(const std::string name, int w, int h, bool with_depth = true);

        ~Frame_Buffer();

        // No copiable
        Frame_Buffer(const Frame_Buffer&) = delete;
        Frame_Buffer& operator=(const Frame_Buffer&) = delete;

        // Movible
        /// @brief Constructor de movimiento. Transfiere la propiedad de los recursos OpenGL.
        Frame_Buffer(Frame_Buffer&& other) noexcept;
        /// @brief Operador de asignación por movimiento.
        Frame_Buffer& operator=(Frame_Buffer&& other) noexcept;

        /// @brief Vincula el FBO para renderizar en él.
        void bind() const;
        /// @brief Vincula la textura de color a una unidad de textura.
        /// @param unit Unidad de textura (0 por defecto).
        void bind_color_texture(int unit) const;
        /// @brief Desvincula el FBO actual (vuelve al framebuffer por defecto).
        void unbind() const;
        /// @brief Redimensiona la textura de color y el renderbuffer de profundidad.
        /// @param w Nuevo ancho.
        /// @param h Nuevo alto.
        void resize(int w, int h);

    private:
        /// @brief Libera todos los recursos OpenGL asociados.
        void release();
        /// @brief Crea el FBO, la textura de color y opcionalmente el renderbuffer.
        /// @param with_depth Si true, crea el renderbuffer de profundidad/stencil.
        void create_framebuffer_object(bool with_depth);
    };
}