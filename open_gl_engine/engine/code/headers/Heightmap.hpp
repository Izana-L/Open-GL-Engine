#pragma once

#include <vector>
#include <string>

namespace open_gl_engine
{
    /// @brief Estructura que almacena los datos de un mapa de alturas (heightmap).
    /// Contiene las dimensiones en píxeles, los valores normalizados (0–1)
    /// y los parámetros de escala para el mundo 3D.
    struct Heightmap
    {
        int width = 0;                ///< Ancho en píxeles de la imagen del heightmap.
        int height = 0;               ///< Alto en píxeles.
        float max_world_height = 10.0f; ///< Altura máxima en unidades del mundo.
        float world_size_xz = 100.0f;   ///< Tamaño total del terreno en los ejes X y Z.
        std::vector<float> data;        ///< Valores de altura normalizados entre 0.0 y 1.0.

        /// @brief Carga un heightmap desde un archivo de imagen (PNG, BMP, etc.).
        /// @param path Ruta al archivo de imagen.
        /// @param max_height Altura máxima en unidades del mundo.
        /// @param world_size Tamaño en XZ del terreno en unidades del mundo.
        /// @return Instancia de Heightmap con los datos cargados y normalizados.
        static Heightmap load_from_image(const std::string& path, float max_height, float world_size);
    };
}