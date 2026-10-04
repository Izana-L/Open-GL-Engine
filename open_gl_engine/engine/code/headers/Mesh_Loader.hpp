#pragma once

#include <vector>
#include <Vertex.hpp>   
#include <Heightmap.hpp> 

struct aiMesh;

/// @brief Funciones de carga de geometría (vértices e índices) para mallas.
/// Proporciona generadores de primitivas y funciones de importación desde Assimp y heightmaps.
namespace open_gl_engine
{
    // ---- Generación de vértices ----

    /// @brief Convierte los datos de una aiMesh importada a un vector de vértices.
    std::vector<Vertex> load_imported_mesh(const aiMesh& imported_mesh);
    /// @brief Vértices de un cono (8 secciones).
    std::vector<Vertex> load_cone_mesh();
    /// @brief Vértices de un plano (3x3 tiles).
    std::vector<Vertex> load_plane_mesh();
    /// @brief Vértices de un cilindro (8 secciones).
    std::vector<Vertex> load_cilinder_mesh();
    /// @brief Vértices de una esfera (20 sectores, 10 stacks).
    std::vector<Vertex> load_sphere_mesh();
    /// @brief Vértices de un cubo (6 caras con normales separadas).
    std::vector<Vertex> load_cube_mesh();
    /// @brief Vértices de un terreno a partir de un heightmap.
    std::vector<Vertex> load_terrain_mesh(const Heightmap& heightmap);
    /// @brief Vértices de un cubo para skybox (solo posiciones, normales y UV no usadas).
    std::vector<Vertex> load_skybox_cube_mesh();
    /// @brief Vértices de un quad (dos triángulos).
    std::vector<Vertex> load_quad_mesh();

    // ---- Generación de índices ----

    /// @brief Índices para una aiMesh importada (asume triángulos).
    std::vector<unsigned int> load_imported_mesh_index(const aiMesh& imported_mesh);
    /// @brief Índices del cono.
    std::vector<unsigned int> load_cone_mesh_index();
    /// @brief Índices del plano.
    std::vector<unsigned int> load_plane_mesh_index();
    /// @brief Índices del cilindro.
    std::vector<unsigned int> load_cilinder_mesh_index();
    /// @brief Índices de la esfera.
    std::vector<unsigned int> load_sphere_mesh_index();
    /// @brief Índices del cubo.
    std::vector<unsigned int> load_cube_mesh_index();
    /// @brief Índices del terreno.
    std::vector<unsigned int> load_terrain_mesh_index(const Heightmap& heightmap);
    /// @brief Índices del cubo de skybox.
    std::vector<unsigned int> load_skybox_cube_mesh_index();
    /// @brief Índices del quad.
    std::vector<unsigned int> load_quad_mesh_index();
}