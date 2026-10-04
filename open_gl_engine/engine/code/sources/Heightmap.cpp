#include <Heightmap.hpp>
#include <SOIL2.h>
#include <cassert>
#include <algorithm>
#include <Texture_Manager.hpp>

namespace open_gl_engine
{
    Heightmap Heightmap::load_from_image(const std::string& path, float max_height, float world_size)
    {
        Heightmap heightmap;
        // Guarda los parámetros de escalado en el mundo
        heightmap.max_world_height = max_height;
        heightmap.world_size_xz = world_size;

        // Carga la imagen usando el gestor de texturas (devuelve datos RGBA)
        const TextureData imgen = Texture_Manager::get().load_texture_data(path);

        // Dimensiones en píxeles del heightmap
        heightmap.width = imgen.width;
        heightmap.height = imgen.height;
        heightmap.data.resize(heightmap.width * heightmap.height);

        // Convierte cada píxel a un valor de gris normalizado [0,1]
        for (size_t i = 0; i < heightmap.data.size(); ++i)
        {
            unsigned char* pixel = imgen.pixels + i * 4;  // Avance RGBA
            // Fórmula estándar de luminosidad
            float gray = 0.299f * pixel[0] + 0.587f * pixel[1] + 0.114f * pixel[2];
            heightmap.data[i] = gray / 255.0f;
        }

        // Libera la memoria de la imagen cruda
        SOIL_free_image_data(imgen.pixels);
        return heightmap;
    }
}