#include "Cubemap.hpp"
#include <glad/gl.h>

namespace open_gl_engine
{
    // Constructor por defecto: el cubemap se crea sin recurso asignado.
    Cubemap::Cubemap() : id(0), loaded(false) {}

    // Constructor explícito: inicializa con un ID ya existente y lo marca como cargado.
    Cubemap::Cubemap(unsigned int _id) : id(_id), loaded(true) {}

    // Constructor de movimiento: transfiere el id y el estado, dejando al origen inválido.
    Cubemap::Cubemap(Cubemap&& other) noexcept
        : id(other.id), loaded(other.loaded)
    {
        other.id = 0;
        other.loaded = false;
    }

    // Asignación por movimiento: libera el recurso propio y transfiere el ajeno.
    Cubemap& Cubemap::operator=(Cubemap&& other) noexcept
    {
        if (this != &other) {
            release();                // Libera el recurso actual
            id = other.id;
            loaded = other.loaded;
            other.id = 0;
            other.loaded = false;
        }
        return *this;
    }

    // Destructor: libera automáticamente el recurso OpenGL.
    Cubemap::~Cubemap()
    {
        release();
    }

    // Enlace: activa la unidad de textura deseada y vincula el cubemap.
    void Cubemap::bind(int unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);   // Selecciona la unidad de textura
        glBindTexture(GL_TEXTURE_CUBE_MAP, id); // Vincula el cubemap
    }

    // Desenlace estático: desvincula cualquier cubemap de la unidad dada.
    void Cubemap::unbind(int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // 0 desvincula
    }

    // Liberación: si el recurso está cargado, lo elimina de OpenGL y resetea.
    void Cubemap::release()
    {
        if (loaded && id) {
            glDeleteTextures(1, &id); // Libera el objeto textura
            id = 0;
            loaded = false;
        }
    }
}