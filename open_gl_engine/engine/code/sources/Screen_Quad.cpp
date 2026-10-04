#include <Screen_Quad.hpp>
#include <glad/gl.h>

namespace open_gl_engine 
{
    GLuint ScreenQuad::vao_id = 0;
    GLuint ScreenQuad::vbo_id = 0;
    bool ScreenQuad::initialized = false;

    GLuint ScreenQuad::vao() 
    {
        if (!initialized) 
        {
            float quadVertices[] = 
            {
                // positions   // texCoords
                -1.0f,  1.0f,  0.0f, 1.0f,  // v0 arriba-izq
                -1.0f, -1.0f,  0.0f, 0.0f,  // v1 abajo-izq
                 1.0f,  1.0f,  1.0f, 1.0f,  // v5 arriba-der (usado en 2º triángulo)
                -1.0f, -1.0f,  0.0f, 0.0f,  // v1
                 1.0f, -1.0f,  1.0f, 0.0f,  // v2 abajo-der
                 1.0f,  1.0f,  1.0f, 1.0f   // v5
            };
            glGenVertexArrays(1, &vao_id);
            glGenBuffers(1, &vbo_id);
            glBindVertexArray(vao_id);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
            glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
            glBindVertexArray(0);
            initialized = true;
        }
        return vao_id;
    }

    void ScreenQuad::destroy() 
    {
        if (initialized) {
            glDeleteVertexArrays(1, &vao_id);
            glDeleteBuffers(1, &vbo_id);
            vao_id = 0;
            vbo_id = 0;
            initialized = false;
        }
    }
}