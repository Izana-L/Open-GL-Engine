#pragma once

#include <cstddef>         

namespace open_gl_engine
{
    /// @brief Envoltura RAII para una malla de OpenGL.
    /// Almacena un VAO, dos VBOs (posiciones y normales/UVs) y el número de índices.
    /// No copiable, solo movible.
    struct Mesh
    {
        unsigned int vao_id;        ///< ID del Vertex Array Object de OpenGL.
        unsigned int vbo_ids[2];    ///< IDs de los Vertex Buffer Objects (posiciones, atributos).
        int index_count;            ///< Número de índices a dibujar (glDrawElements).

        /// @brief Constructor por defecto. Inicializa todos los identificadores a 0.
        Mesh() : vao_id(0), vbo_ids{ 0, 0 }, index_count(0) {}

        // No copiable
        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;

        // Movible
        /// @brief Constructor de movimiento. Transfiere los recursos OpenGL.
        Mesh(Mesh&& other) noexcept;
        /// @brief Operador de asignación por movimiento.
        Mesh& operator=(Mesh&& other) noexcept;

        /// @brief Destructor. Libera el VAO y los VBOs.
        ~Mesh();

    private:
        /// @brief Libera todos los recursos OpenGL asociados.
        void release();
    };
}