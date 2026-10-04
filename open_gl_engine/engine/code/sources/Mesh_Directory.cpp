#include <Mesh_Directory.hpp>
#include <Mesh_Loader.hpp>
#include <Heightmap.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glad/gl.h>
#include <cassert>

namespace open_gl_engine
{
    // --- Primitivas con caché ---

    const Mesh* Mesh_Directory::create_cone_mesh()
    {
        const std::string key = "default_cone";
        auto it = mesh_cache.find(key);
        if (it != mesh_cache.end()) return it->second; // ya existe

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_cone_mesh(), load_cone_mesh_index()); // subir geometría
        Mesh* ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[key] = ptr; // cachear
        return ptr;
    }

    const Mesh* Mesh_Directory::create_plane_mesh()
    {
        const std::string key = "default_plane";
        auto it = mesh_cache.find(key);
        if (it != mesh_cache.end()) return it->second;

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_plane_mesh(), load_plane_mesh_index());
        Mesh* ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[key] = ptr;
        return ptr;
    }

    const Mesh* Mesh_Directory::create_cilinder_mesh()
    {
        const std::string key = "default_plane"; // nota: clave reutilizada
        auto it = mesh_cache.find(key);
        if (it != mesh_cache.end()) return it->second;

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_cilinder_mesh(), load_cilinder_mesh_index());
        Mesh* ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[key] = ptr;
        return ptr;
    }

    const Mesh* Mesh_Directory::create_sphere_mesh()
    {
        const std::string key = "default_sphere";
        auto it = mesh_cache.find(key);
        if (it != mesh_cache.end()) return it->second;

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_sphere_mesh(), load_sphere_mesh_index());
        Mesh* ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[key] = ptr;
        return ptr;
    }

    const Mesh* Mesh_Directory::create_cube_mesh()
    {
        const std::string key = "default_cube";
        auto it = mesh_cache.find(key);
        if (it != mesh_cache.end()) return it->second;

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_cube_mesh(), load_cube_mesh_index());
        Mesh* ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[key] = ptr;
        return ptr;
    }
    const Mesh* Mesh_Directory::get_or_create_mesh(const std::string& cache_key, const aiMesh& imported_mesh)
    {
        auto it = mesh_cache.find(cache_key);
        if (it != mesh_cache.end()) return it->second;
        
        const Mesh* new_mesh = create_imported_mesh(imported_mesh);
        mesh_cache[cache_key] = const_cast<Mesh*>(new_mesh);
        return new_mesh;
    }
    const Mesh* Mesh_Directory::create_skybox_mesh()
    {
        const std::string key = "default_skybox_mesh";
        auto it = mesh_cache.find(key);
        if (it != mesh_cache.end()) return it->second;

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_skybox_cube_mesh(), load_skybox_cube_mesh_index());
        Mesh* ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[key] = ptr;
        return ptr;
    }

    const Mesh* Mesh_Directory::create_quad_mesh()
    {
        const std::string key = "default_quad";
        auto it = mesh_cache.find(key);
        if (it != mesh_cache.end()) return it->second;

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_quad_mesh(), load_quad_mesh_index());
        Mesh* ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[key] = ptr;
        return ptr;
    }

    // --- Mallas sin caché (se crean cada vez) ---

    const Mesh* Mesh_Directory::create_imported_mesh(const aiMesh& imported_mesh)
    {
        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_imported_mesh(imported_mesh), load_imported_mesh_index(imported_mesh));
        meshes.push_back(std::move(mesh));
        return meshes.back().get();
    }

    const Mesh* Mesh_Directory::create_terrain_mesh(const Heightmap& heightmap)
    {
        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_terrain_mesh(heightmap), load_terrain_mesh_index(heightmap));
        meshes.push_back(std::move(mesh));
        return meshes.back().get();
    }

    // --- Importación con caché ---

    const Mesh* Mesh_Directory::import_mesh(std::string filename, unsigned int flags)
    {
        auto it = mesh_cache.find(filename);
        if (it != mesh_cache.end())
            return it->second; // cache hit

        std::string full_path = mesh_base_path + filename;

        Assimp::Importer importer;
        const aiScene* imported_scene = importer.ReadFile(full_path, flags);

        assert(imported_scene != nullptr);
        assert(imported_scene->mRootNode != nullptr);
        assert(imported_scene->mNumMeshes > 0);

        aiMesh* imported_mesh = imported_scene->mMeshes[0]; // primera malla

        auto mesh = std::make_unique<Mesh>();
        upload_mesh(*mesh, load_imported_mesh(*imported_mesh), load_imported_mesh_index(*imported_mesh));

        Mesh* raw_ptr = mesh.get();
        meshes.push_back(std::move(mesh));
        mesh_cache[filename] = raw_ptr; // cachear
        return raw_ptr;
    }

    // --- Subida a GPU ---

    void Mesh_Directory::upload_mesh(Mesh& mesh, const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        // Generar buffers y VAO
        glGenBuffers(2, mesh.vbo_ids);
        glGenVertexArrays(1, &mesh.vao_id);

        glBindVertexArray(mesh.vao_id);

        // VBO de vértices
        glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo_ids[0]);
        glBufferData(GL_ARRAY_BUFFER,
            vertices.size() * sizeof(Vertex),
            vertices.data(),
            GL_STATIC_DRAW);

        // Configurar atributos (posición, normal, uv)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)12);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)24);

        // IBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.vbo_ids[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
            indices.size() * sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW);

        mesh.index_count = static_cast<unsigned int>(indices.size());

        glBindVertexArray(0); // desvincular
    }
}