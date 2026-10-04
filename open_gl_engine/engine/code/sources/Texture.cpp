// Texture.cpp
#include "Texture.hpp"
#include <glad/gl.h>

namespace open_gl_engine
{
    // Constructor de movimiento: transfiere el ID, dimensiones y canales, y deja al origen inválido.
    Texture::Texture(Texture&& other) noexcept
        : id(other.id), width(other.width), height(other.height), channels(other.channels)
    {
        other.id = 0; // El objeto origen ya no posee la textura
    }

    // Asignación por movimiento: libera el recurso propio y copia los datos del otro, luego invalida el otro.
    Texture& Texture::operator=(Texture&& other) noexcept
    {
        if (this != &other) {
            release();               // Libera la textura actual si existe
            id = other.id;
            width = other.width;
            height = other.height;
            channels = other.channels;
            other.id = 0;            // El origen queda sin recurso
        }
        return *this;
    }

    // Destructor: libera automáticamente el recurso OpenGL.
    Texture::~Texture() { release(); }

    // Vincula la textura a la unidad de textura especificada.
    void Texture::bind(unsigned int unit) const
    {
        glActiveTexture(GL_TEXTURE0 + unit);   // Selecciona la unidad activa
        glBindTexture(GL_TEXTURE_2D, id);      // Enlaza esta textura
    }

    // Desvincula cualquier textura de la unidad dada (estática).
    void Texture::unbind(unsigned int unit)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, 0);       // 0 desvincula
    }

    // Libera la textura OpenGL si el ID no es 0.
    void Texture::release()
    {
        if (id) {
            glDeleteTextures(1, &id);          // Borra la textura en OpenGL
            id = 0;                            // Marca como liberada
        }
    }
}