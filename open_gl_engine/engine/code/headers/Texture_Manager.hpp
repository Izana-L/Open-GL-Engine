#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <Cubemap.hpp>
#include <Texture.hpp>

namespace open_gl_engine
{
    /// @brief Datos crudos de una textura cargada (píxeles y dimensiones).
    struct TextureData
    {
        unsigned char* pixels; ///< Puntero a los datos RGBA (liberar con SOIL_free_image_data).
        int width;             ///< Ancho en píxeles.
        int height;            ///< Alto en píxeles.
    };

    /// @brief Singleton que gestiona la carga, almacenamiento y liberación de texturas 2D y cubemaps.
    /// Proporciona caché por ruta y texturas por defecto (blanca, cubemap por defecto).
    class Texture_Manager
    {
    private:
        std::vector<std::unique_ptr<Texture>> textures;                    ///< Almacén propietario de texturas 2D.
        std::vector<std::unique_ptr<Cubemap>> cubemaps;                    ///< Almacén propietario de cubemaps.
        std::unordered_map<std::string, Texture*> path_cache_texture;      ///< Caché nombre -> textura 2D.
        std::unordered_map<std::string, Cubemap*> path_cache_cube;         ///< Caché nombre -> cubemap.
        const Cubemap* default_cubemap;                                    ///< Cubemap por defecto (blanco 1x1).
        const Texture* white_texture;                                      ///< Textura 2D blanca 1x1 por defecto.
        std::string base_path;                                             ///< Ruta base para la carga de texturas.

        Texture_Manager();
        ~Texture_Manager() = default;

    public:
        /// @brief Obtiene la instancia única.
        static Texture_Manager& get();

        /// @brief Devuelve la textura blanca 1x1 (la crea si es necesario).
        const Texture* get_white_texture();

        /// @brief Devuelve el cubemap por defecto (blanco 1x1).
        const Cubemap* get_default_cubemap();

        /// @brief Carga una textura 2D desde archivo, con caché.
        /// @param filename Nombre del archivo (relativo a base_path).
        /// @return Puntero a la Texture cargada, o textura blanca si falla.
        const Texture* load_texture(const std::string& filename);

        /// @brief Carga los datos crudos de una textura (sin subir a GPU ni cachear).
        /// @param filename Nombre del archivo.
        /// @return TextureData con los píxeles y dimensiones.
        const TextureData load_texture_data(const std::string& filename);

        /// @brief Carga un cubemap a partir de 6 archivos de cara.
        /// @param face_filenames Vector con los 6 nombres de archivo (orden: -Z, -X, +Z, +X, +Y, -Y).
        /// @return Puntero al Cubemap cargado, o cubemap por defecto si falla.
        const Cubemap* load_cubemap(const std::vector<std::string>& face_filenames);

        // No copiable
        Texture_Manager(const Texture_Manager&) = delete;
        Texture_Manager& operator=(const Texture_Manager&) = delete;

    private:
        /// @brief Sube datos de textura 2D a la GPU y devuelve el ID de OpenGL.
        static unsigned int upload_to_gpu_texture(const unsigned char* data, int w, int h, int channels);

        /// @brief Sube 6 caras a un cubemap OpenGL y devuelve el ID generado.
        static unsigned int upload_cubemap(const unsigned char* faces[6], int width, int height, int channels);
    };
}