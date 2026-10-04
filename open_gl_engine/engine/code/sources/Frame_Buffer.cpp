#include <Frame_Buffer.hpp>
#include <glad/gl.h>
#include <cassert>

namespace open_gl_engine
{
    // Constructor principal: inicializa dimensiones y construye el FBO con sus adjuntos.
    Frame_Buffer::Frame_Buffer(const std::string name, int w, int h, bool with_depth)
        : name(name), width(w), height(h)
    {
        create_framebuffer_object(with_depth);
    }

    // Constructor de movimiento: transfiere los IDs de OpenGL y resetea el origen.
    Frame_Buffer::Frame_Buffer(Frame_Buffer&& other) noexcept
        : framebuffer_object_id(other.framebuffer_object_id),
        texture_id(other.texture_id),
        renderbuffer_object_id(other.renderbuffer_object_id),
        width(other.width),
        height(other.height),
        name(std::move(other.name))
    {
        other.framebuffer_object_id = 0;
        other.texture_id = 0;
        other.renderbuffer_object_id = 0;
    }

    // Asignación por movimiento: libera recursos propios y transfiere los del otro.
    Frame_Buffer& Frame_Buffer::operator=(Frame_Buffer&& other) noexcept
    {
        if (this != &other)
        {
            release();
            framebuffer_object_id = other.framebuffer_object_id;
            texture_id = other.texture_id;
            renderbuffer_object_id = other.renderbuffer_object_id;
            width = other.width;
            height = other.height;
            name = std::move(other.name);

            other.framebuffer_object_id = 0;
            other.texture_id = 0;
            other.renderbuffer_object_id = 0;
        }
        return *this;
    }

    // Destructor: libera automáticamente los recursos OpenGL.
    Frame_Buffer::~Frame_Buffer()
    {
        release();
    }

    // Vincula el FBO para que las operaciones de dibujo se realicen sobre él.
    void Frame_Buffer::bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id);
    }

    // Desvincula el FBO, volviendo al framebuffer por defecto (pantalla).
    void Frame_Buffer::unbind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Redimensiona la textura de color y, si existe, el renderbuffer de profundidad.
    void Frame_Buffer::resize(int w, int h)
    {
        assert(texture_id != 0);
        if (w == width && h == height) return; // Sin cambios
        width = w;
        height = h;

        // Actualiza la textura de color con nuevas dimensiones
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Si hay renderbuffer, ajusta su almacenamiento
        if (renderbuffer_object_id != 0)
        {
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object_id);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
            glBindRenderbuffer(GL_RENDERBUFFER, 0);
        }
    }

    // Crea el FBO con una textura de color y, opcionalmente, un renderbuffer de profundidad.
    void Frame_Buffer::create_framebuffer_object(bool with_depth)
    {
        // 1. Textura de color
        glGenTextures(1, &texture_id);
        glBindTexture(GL_TEXTURE_2D, texture_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);

        // 2. Crear FBO y adjuntar textura de color
        glGenFramebuffers(1, &framebuffer_object_id);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id, 0);

        // 3. Opcional: renderbuffer de profundidad/stencil
        if (with_depth)
        {
            glGenRenderbuffers(1, &renderbuffer_object_id);
            glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object_id);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_object_id);
        }

        // 4. Verificar integridad
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            assert(false && "Framebuffer incompleto");
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    // Vincula la textura de color a una unidad de textura concreta.
    void Frame_Buffer::bind_color_texture(int unit) const
    {
        assert(texture_id != 0 && "FBO sin textura de color");
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

    // Libera todos los objetos OpenGL asociados (FBO, textura, renderbuffer).
    void Frame_Buffer::release()
    {
        if (framebuffer_object_id) glDeleteFramebuffers(1, &framebuffer_object_id);
        if (texture_id)            glDeleteTextures(1, &texture_id);
        if (renderbuffer_object_id) glDeleteRenderbuffers(1, &renderbuffer_object_id);
        framebuffer_object_id = 0;
        texture_id = 0;
        renderbuffer_object_id = 0;
    }
}