#include <cassert>
#include <algorithm>
#include <glad/gl.h>
#include <SOIL2.h>
#include <Texture_Manager.hpp>

namespace open_gl_engine
{
    // Singleton.
    Texture_Manager& Texture_Manager::get()
    {
        static Texture_Manager instance;
        return instance;
    }

    // Constructor: define ruta base e inicializa punteros a nullptr.
    Texture_Manager::Texture_Manager()
        : default_cubemap(nullptr), white_texture(nullptr), base_path("../assets/textures/")
    {
    }

    // Obtiene la textura blanca 1x1. La crea bajo demanda.
    const Texture* Texture_Manager::get_white_texture()
    {
        if (white_texture) return white_texture;

        // Crear píxel blanco RGBA
        unsigned char white[4] = { 255, 255, 255, 255 };
        unsigned int id = upload_to_gpu_texture(white, 1, 1, 4);
        auto tex = std::make_unique<Texture>(id, 1, 1, 4);
        white_texture = tex.get();
        textures.push_back(std::move(tex));
        return white_texture;
    }

    // Carga una textura 2D desde archivo. Usa caché.
    const Texture* Texture_Manager::load_texture(const std::string& filename)
    {
        // Verificar caché
        auto it = path_cache_texture.find(filename);
        if (it != path_cache_texture.end())
            return it->second;

        std::string full_path = base_path + filename;

        // Cargar con SOIL2 forzando RGBA
        int image_width, image_height, image_channels;
        unsigned char* loaded_pixels = SOIL_load_image(full_path.c_str(), &image_width, &image_height, &image_channels, SOIL_LOAD_RGBA);
        if (!loaded_pixels) {
            // Fallback: devolver textura blanca
            return white_texture;
        }

        // Detectar si hay píxeles con alfa < 255 (translucidez)
        bool has_alpha = false;
        if (image_channels == 4) {
            for (int i = 3; i < image_width * image_height * 4; i += 4) {
                if (loaded_pixels[i] < 255) {
                    has_alpha = true;
                    break;
                }
            }
        }

        // Subir a GPU y liberar datos de CPU
        unsigned int id = upload_to_gpu_texture(loaded_pixels, image_width, image_height, 4);
        SOIL_free_image_data(loaded_pixels);

        // Crear objeto Texture, cachear y almacenar
        auto texture = std::make_unique<Texture>(id, image_width, image_height, 4);
        texture->has_alpha_less_than_one = has_alpha;
        Texture* ptr_texture = texture.get();
        textures.push_back(std::move(texture));
        path_cache_texture[filename] = ptr_texture;
        return ptr_texture;
    }

    // Carga datos crudos de una textura (sin cachear, sin subir a GPU). El llamante debe liberar con SOIL_free_image_data.
    const TextureData Texture_Manager::load_texture_data(const std::string& filename)
    {
        std::string full_path = base_path + filename;
        int image_width, image_height, image_channels;
        unsigned char* loaded_pixels = SOIL_load_image(full_path.c_str(), &image_width, &image_height, &image_channels, SOIL_LOAD_RGBA);
        return { loaded_pixels, image_width, image_height };
    }

    // Carga un cubemap desde 6 archivos. En caso de error o caras de distinto tamaño, devuelve el cubemap por defecto.
    const Cubemap* Texture_Manager::load_cubemap(const std::vector<std::string>& face_filenames) {
        assert(face_filenames.size() == 6 && "Se requieren 6 caras para un cubemap");

        // Verificar caché (usamos el primer nombre como clave)
        auto it = path_cache_cube.find(face_filenames[0]);
        if (it != path_cache_cube.end())
            return it->second;

        // Cargar las 6 imágenes
        std::vector<TextureData> faces(6);
        int common_width = -1, common_height = -1;

        for (int i = 0; i < 6; ++i) {
            std::string full_path = base_path + face_filenames[i];
            int width, height, channels;
            unsigned char* pixels = SOIL_load_image(full_path.c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);
            if (!pixels) {
                // Si falla, liberar las caras ya cargadas y devolver el cubemap por defecto
                for (int j = 0; j < i; ++j) {
                    SOIL_free_image_data(faces[j].pixels);
                }
                return get_default_cubemap();
            }
            faces[i] = { pixels, width, height };

            if (i == 0) {
                common_width = width;
                common_height = height;
            }
            else if (width != common_width || height != common_height) {
                // Liberar todo y devolver cubemap por defecto
                for (int j = 0; j <= i; ++j) {
                    SOIL_free_image_data(faces[j].pixels);
                }
                return get_default_cubemap();
            }
        }

        // Preparar array de punteros para upload
        const unsigned char* face_ptrs[6];
        for (int i = 0; i < 6; ++i) {
            face_ptrs[i] = faces[i].pixels;
        }

        // Subir a GPU
        unsigned int id = upload_cubemap(face_ptrs, common_width, common_height, 4);

        // Liberar datos de CPU
        for (int i = 0; i < 6; ++i) {
            SOIL_free_image_data(faces[i].pixels);
        }

        // Crear recurso Cubemap, almacenar y cachear
        auto cubemap = std::make_unique<Cubemap>(id);
        Cubemap* ptr = cubemap.get();
        cubemaps.push_back(std::move(cubemap));
        path_cache_cube[face_filenames[0]] = ptr; // key arbitraria: el primer nombre

        return ptr;
    }

    // Cubemap por defecto (blanco 1x1). Se crea bajo demanda.
    const Cubemap* Texture_Manager::get_default_cubemap()
    {
        if (default_cubemap) return default_cubemap;

        unsigned char white[4] = { 255, 255, 255, 255 };
        const unsigned char* faces[6] = { white, white, white, white, white, white };

        unsigned int id = upload_cubemap(faces, 1, 1, 4);

        auto cubemap = std::make_unique<Cubemap>(id);
        default_cubemap = cubemap.get();
        cubemaps.push_back(std::move(cubemap));
        return default_cubemap;
    }

    // Sube una textura 2D a la GPU. Genera mipmaps si la textura es mayor de 1x1.
    unsigned int Texture_Manager::upload_to_gpu_texture(const unsigned char* data, int w, int h, int channels)
    {
        GLuint tex_id;
        glGenTextures(1, &tex_id);
        glBindTexture(GL_TEXTURE_2D, tex_id);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);

        // Filtros y wrapping
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        if (w > 1 && h > 1)
            glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);
        return tex_id;
    }

    // Sube un cubemap a la GPU con las 6 caras. Genera mipmaps si es necesario.
    unsigned int Texture_Manager::upload_cubemap(const unsigned char* faces[6], int width, int height, int channels)
    {
        GLuint id;
        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_CUBE_MAP, id);

        // Configuración estándar
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;

        // Orden de caras: -Z, -X, +Z, +X, +Y, -Y
        static const GLenum texture_target[] =
        {
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
            GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
        };

        for (int i = 0; i < 6; ++i) {
            glTexImage2D(texture_target[i], 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, faces[i]);
        }

        if (width > 1 && height > 1)
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        return id;
    }
}