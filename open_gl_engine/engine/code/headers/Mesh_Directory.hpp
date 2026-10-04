#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <assimp/postprocess.h> 
#include <Vertex.hpp>
#include <Heightmap.hpp>
#include <Mesh.hpp>   

struct aiMesh;

namespace open_gl_engine
{
    /// @brief Singleton que crea, almacena y cachea mallas (primitivas, importadas, terreno, etc.).
    /// Proporciona acceso a mallas por clave y gestiona la subida de geometría a la GPU.
    class Mesh_Directory
    {
    private:
        Mesh_Directory() = default;

        std::vector<std::unique_ptr<Mesh>> meshes;          ///< Almacén propietario de todas las mallas creadas.
        std::unordered_map<std::string, Mesh*> mesh_cache;  ///< Caché nombre -> puntero a malla.
        std::string mesh_base_path = "../assets/meshs/";    ///< Ruta base para importación de archivos.

        /// @brief Flags por defecto para la importación con Assimp.
        static constexpr unsigned int default_flags =
            aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes |
            aiProcess_OptimizeGraph | aiProcess_FlipUVs;

    public:
        Mesh_Directory(const Mesh_Directory&) = delete;
        Mesh_Directory& operator=(const Mesh_Directory&) = delete;

        /// @brief Obtiene la instancia única del directorio.
        static Mesh_Directory& get()
        {
            static Mesh_Directory instance;
            return instance;
        }
        const std::string& get_base_path() const { return mesh_base_path; }
        /// @name Primitivas con caché
        /// @{
        const Mesh* create_cone_mesh();
        const Mesh* create_plane_mesh();
        const Mesh* create_cilinder_mesh();
        const Mesh* create_sphere_mesh();
        const Mesh* create_cube_mesh();
        /// @}
        const Mesh* get_or_create_mesh(const std::string& cache_key, const aiMesh& imported_mesh);
        /// @brief Crea una malla a partir de una aiMesh importada (sin caché).
        const Mesh* create_imported_mesh(const aiMesh& imported_mesh);

        /// @brief Importa una malla desde archivo usando Assimp (con caché).
        const Mesh* import_mesh(std::string filename, unsigned int flags = default_flags);

        /// @brief Crea una malla de terreno desde un heightmap (sin caché).
        const Mesh* create_terrain_mesh(const Heightmap& heightmap);

        /// @brief Crea malla de skybox y quad con caché.
        const Mesh* create_skybox_mesh();
        const Mesh* create_quad_mesh();

    private:
        /// @brief Sube vértices e índices a la GPU configurando el VAO y VBOs.
        void upload_mesh(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    };
}