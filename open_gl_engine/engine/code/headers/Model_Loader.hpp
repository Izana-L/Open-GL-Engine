#pragma once
#include <string>
#include <vector>
#include <Id.hpp>

namespace open_gl_engine
{
    class Scene;
    /// @brief Carga un modelo 3D (FBX, OBJ, etc.) respetando la jerarquía de nodos de Assimp.
    /// @param scene Escena donde se crearán las entidades.
    /// @param filename Nombre del archivo (ej. "coche.obj"). La ruta completa se construye con la base de Mesh_Directory.
    /// @param flags Banderas de importación de Assimp (por defecto las mismas que Mesh_Directory).
    /// @return Id de la entidad raíz (contenedor del modelo completo).
    Id load_model(Scene& scene, const std::string& filename, unsigned int flags = 0); // 0 indica usar default_flags
    Id load_model_with_texture(Scene& scene,const std::string& filename_mesh,const std::string& filename_texture, unsigned int flags=0);
}