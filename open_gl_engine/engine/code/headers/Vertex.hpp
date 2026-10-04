#pragma once
#include <glm.hpp>

/// @brief Estructura que representa un vértice con posición, normal y coordenadas de textura.
/// Utilizada para construir mallas y subir geometría a la GPU.
struct Vertex
{
    glm::vec3 coordinates;         ///< Posición del vértice en espacio local.
    glm::vec3 normals;             ///< Normal del vértice.
    glm::vec2 texture_coordinates; ///< Coordenadas UV (-1 por defecto indica "sin asignar").

    /// @brief Constructor completo.
    /// @param coord Posición.
    /// @param norm Normal.
    /// @param texture_coord Coordenada UV (por defecto -1).
    Vertex(const glm::vec3& coord, const glm::vec3& norm, const glm::vec2& texture_coord = glm::vec2(-1.0f))
        : coordinates(coord), normals(norm), texture_coordinates(texture_coord) {
    }

    /// @brief Constructor por defecto. Inicializa con (0,0,0), normal (0,0,0) y UV (-1,-1).
    Vertex()
    {
        coordinates = glm::vec3(0.0f);
        normals = glm::vec3(0.0f);
        texture_coordinates = glm::vec2(-1.0f);
    }
};