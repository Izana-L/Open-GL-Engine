#include "Model_Loader.hpp"
#include <Scene.hpp>
#include <Transform_Component.hpp>
#include <Mesh_Component.hpp>
#include <Texture_Manager.hpp>
#include <Texture_Component.hpp>
#include <Mesh_Directory.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <cassert>

namespace open_gl_engine 
{
    static constexpr unsigned int kDefaultImportFlags =
        aiProcess_Triangulate | aiProcess_GenSmoothNormals |
        aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices |
        aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes |
        aiProcess_OptimizeGraph | aiProcess_FlipUVs;

    static unsigned int resolve_flags(unsigned int user_flags) {
        return user_flags ? user_flags : kDefaultImportFlags;
    }

    static void decompose_assimp_matrix(const aiMatrix4x4& ai_mat, glm::vec3& pos, glm::quat& rot, glm::vec3& scale)
    {
        aiMatrix4x4 transposed = ai_mat;
        transposed.Transpose();

        glm::mat4 glm_mat(
            transposed.a1, transposed.b1, transposed.c1, transposed.d1,
            transposed.a2, transposed.b2, transposed.c2, transposed.d2,
            transposed.a3, transposed.b3, transposed.c3, transposed.d3,
            transposed.a4, transposed.b4, transposed.c4, transposed.d4
        );

        pos = glm::vec3(glm_mat[3]);

        scale.x = glm::length(glm::vec3(glm_mat[0]));
        scale.y = glm::length(glm::vec3(glm_mat[1]));
        scale.z = glm::length(glm::vec3(glm_mat[2]));

        glm::mat3 rot_mat(glm_mat);
        rot_mat[0] /= scale.x;
        rot_mat[1] /= scale.y;
        rot_mat[2] /= scale.z;

        rot = glm::quat_cast(rot_mat);
    }

    static Id process_node(Scene& scene,
        const std::string& full_path,
        aiNode* node,
        const aiScene* assimp_scene,
        Transform_Component* parent_transform,
        const Texture* default_texture = nullptr)
    {
        // 1. Crear entidad contenedora del nodo
        Id node_id = scene.add_entity();

        // 2. Transformación local
        scene.add_component<Transform_Component>(node_id);
        auto trans_opt = scene.get_component<Transform_Component>(node_id);
        assert(trans_opt.has_value());
        Transform_Component& node_trans = trans_opt->get();

        glm::vec3 pos, scale;
        glm::quat rot;
        decompose_assimp_matrix(node->mTransformation, pos, rot, scale);
        node_trans.set_position(pos);
        node_trans.set_rotation_quat(rot);
        node_trans.set_scale(scale);

        if (parent_transform)
            node_trans.set_parent(parent_transform);

        // 3. Mallas del nodo
        for (unsigned int i = 0; i < node->mNumMeshes; ++i)
        {
            unsigned int mesh_idx = node->mMeshes[i];
            aiMesh* aimesh = assimp_scene->mMeshes[mesh_idx];

            // Construir clave de caché y obtener (o crear) la Mesh
            std::string mesh_key = full_path + "/mesh/" + std::to_string(mesh_idx);
            const Mesh* mesh_ptr = Mesh_Directory::get().get_or_create_mesh(mesh_key, *aimesh);

            // Entidad hija para esta malla
            Id mesh_entity = scene.add_entity();
            scene.add_component<Transform_Component>(mesh_entity);
            auto mesh_trans_opt = scene.get_component<Transform_Component>(mesh_entity);
            mesh_trans_opt->get().set_parent(&node_trans);   // hereda del nodo

            scene.add_component<Mesh_Component>(mesh_entity);
            auto mesh_comp_opt = scene.get_component<Mesh_Component>(mesh_entity);
            mesh_comp_opt->get().mesh = mesh_ptr;

            // Si se ha especificado una textura, añadir Texture_Component
            if (default_texture)
            {
                scene.add_component<Texture_Component>(mesh_entity);
                auto tex_comp_opt = scene.get_component<Texture_Component>(mesh_entity);
                tex_comp_opt->get().texture = default_texture;
            }
            // (No se hace nada si no hay textura; el Render_Task usará la blanca por defecto)
        }

        // 4. Procesar hijos recursivamente
        for (unsigned int i = 0; i < node->mNumChildren; ++i)
        {
            process_node(scene, full_path, node->mChildren[i],
                assimp_scene, &node_trans, default_texture);
        }

        return node_id;
    }

    Id load_model(Scene& scene, const std::string& filename, unsigned int flags)
    {
        const unsigned int effective_flags = flags ? flags :
            aiProcess_Triangulate | aiProcess_GenSmoothNormals |
            aiProcess_CalcTangentSpace | aiProcess_JoinIdenticalVertices |
            aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes |
            aiProcess_OptimizeGraph | aiProcess_FlipUVs;

        std::string full_path = Mesh_Directory::get().get_base_path() + filename;

        Assimp::Importer importer;
        const aiScene* assimp_scene = importer.ReadFile(full_path, effective_flags);
        assert(assimp_scene && assimp_scene->mRootNode && assimp_scene->mNumMeshes > 0);

        Id root_id = process_node(scene, full_path, assimp_scene->mRootNode, assimp_scene, nullptr);
        return root_id;
    }
    Id load_model_with_texture(Scene& scene,
        const std::string& filename_mesh,
        const std::string& filename_texture,
        unsigned int flags)
    {
        flags = resolve_flags(flags);
        std::string mesh_base = Mesh_Directory::get().get_base_path();
        std::string full_path = mesh_base + filename_mesh;

        // Cargar la textura una sola vez (si falla, el Texture_Manager devuelve la blanca)
        const Texture* tex = Texture_Manager::get().load_texture(filename_texture);

        Assimp::Importer importer;
        const aiScene* assimp_scene = importer.ReadFile(full_path, flags);
        assert(assimp_scene && assimp_scene->mRootNode && assimp_scene->mNumMeshes > 0);

        return process_node(scene, full_path, assimp_scene->mRootNode, assimp_scene, nullptr, tex);
    }

} // namespace