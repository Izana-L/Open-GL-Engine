#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <Frame_Buffer.hpp>

namespace open_gl_engine
{
    /// @brief Singleton que gestiona framebuffers identificados por nombre.
    /// Permite crear, obtener, eliminar y redimensionar todos los framebuffers
    /// de forma centralizada. Los framebuffers se almacenan como unique_ptr.
    class Frame_Buffer_Manager
    {
    private:
        /// @brief Constructor privado (singleton).
        Frame_Buffer_Manager() = default;

        /// @brief Mapa de framebuffers: nombre -> unique_ptr al Frame_Buffer.
        std::unordered_map<std::string, std::unique_ptr<Frame_Buffer>> frame_buffers;

    public:
        /// @brief Obtiene la instancia única del administrador.
        /// @return Referencia al singleton.
        static Frame_Buffer_Manager& get();

        /// @brief Crea un framebuffer o devuelve uno ya existente con el mismo nombre.
        /// @param id Nombre identificativo.
        /// @param width Ancho en píxeles.
        /// @param height Alto en píxeles.
        /// @param with_depth Si true, incluye renderbuffer de profundidad/stencil.
        /// @return Puntero no propietario al Frame_Buffer creado o existente.
        Frame_Buffer* create_framebuffer(const std::string& id, int width, int height, bool with_depth = true);

        /// @brief Sobrecarga para cadena C.
        /// @param id Nombre (const char*).
        /// @param width Ancho.
        /// @param height Alto.
        /// @param with_depth Profundidad.
        /// @return Puntero al framebuffer.
        Frame_Buffer* create_framebuffer(const char* id, int width, int height, bool with_depth = true)
        {
            return create_framebuffer(std::string(id), width, height, with_depth);
        }

        /// @brief Obtiene un framebuffer por nombre.
        /// @param id Nombre del framebuffer.
        /// @return Puntero al Frame_Buffer, o nullptr si no existe.
        Frame_Buffer* get_framebuffer(const std::string& id) const;

        /// @brief Sobrecarga para cadena C.
        Frame_Buffer* get_framebuffer(const char* id) const
        {
            return get_framebuffer(std::string(id));
        }

        /// @brief Elimina un framebuffer del gestor. Si está vinculado, fuerza su desvinculación.
        /// @param id Nombre del framebuffer a eliminar.
        void remove_framebuffer(const std::string& id);

        /// @brief Sobrecarga para cadena C.
        void remove_framebuffer(const char* id)
        {
            remove_framebuffer(std::string(id));
        }

        /// @brief Redimensiona todos los framebuffers gestionados a un nuevo tamaño.
        /// @param new_width Nuevo ancho en píxeles.
        /// @param new_height Nuevo alto en píxeles.
        void resize_all(int new_width, int new_height);
    };
}