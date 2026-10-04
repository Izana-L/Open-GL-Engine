#include "Mesh.hpp"
#include <glad/gl.h>

namespace open_gl_engine
{
    // Constructor de movimiento: copia los IDs del origen y los pone a cero.
    Mesh::Mesh(Mesh&& other) noexcept
        : vao_id(other.vao_id),
        vbo_ids{ other.vbo_ids[0], other.vbo_ids[1] },
        index_count(other.index_count)
    {
        other.vao_id = 0;
        other.vbo_ids[0] = other.vbo_ids[1] = 0;
        other.index_count = 0;
    }

    // Asignación por movimiento: libera los recursos propios y transfiere los del otro.
    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this != &other)
        {
            release();                     // Libera VAO y VBOs actuales
            vao_id = other.vao_id;
            vbo_ids[0] = other.vbo_ids[0];
            vbo_ids[1] = other.vbo_ids[1];
            index_count = other.index_count;

            // Deja al origen en estado válido vacío
            other.vao_id = 0;
            other.vbo_ids[0] = other.vbo_ids[1] = 0;
            other.index_count = 0;
        }
        return *this;
    }

    // Destructor: libera automáticamente los recursos OpenGL.
    Mesh::~Mesh()
    {
        release();
    }

    // Libera el VAO y los VBOs si sus IDs no son cero.
    void Mesh::release()
    {
        if (vao_id != 0)
        {
            glDeleteVertexArrays(1, &vao_id);
            vao_id = 0;
        }
        if (vbo_ids[0] != 0)
        {
            glDeleteBuffers(1, &vbo_ids[0]);
            vbo_ids[0] = 0;
        }
        if (vbo_ids[1] != 0)
        {
            glDeleteBuffers(1, &vbo_ids[1]);
            vbo_ids[1] = 0;
        }
    }
}