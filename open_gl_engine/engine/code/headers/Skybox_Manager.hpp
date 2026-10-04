#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <glad/gl.h>
#include "Skybox.hpp"
#include "Cubemap.hpp"         // para la función load_cubemap
#include "Mesh.hpp"            // para const Mesh*

namespace open_gl_engine
{
    /// @brief Singleton gestor de skyboxes.
    /// Permite crear y almacenar múltiples skyboxes (día, noche, etc.) identificados por nombre,
    /// y activar uno de ellos para el renderizado de la escena.
    class Skybox_Manager
    {
    private:
        GLuint shared_shader;                                    ///< Programa de shader de skybox compartido por todas las instancias.
        std::unordered_map<std::string, std::unique_ptr<Skybox>> skyboxes; ///< Mapa de nombre a Skybox.
        Skybox* active_skybox = nullptr;                        ///< Skybox actualmente activo para renderizar.

        Skybox_Manager();
        ~Skybox_Manager();

    public:
        /// @brief Obtiene la instancia única del gestor.
        static Skybox_Manager& get();

        /// @brief Añade un nuevo skybox con un nombre dado.
        /// @param name Nombre identificativo (ej. "day", "night").
        /// @param face_filenames Vector con las rutas de las 6 caras del cubemap (en orden).
        /// @return true si el cubemap se cargó correctamente, false en caso de error.
        bool add_skybox(const std::string& name, const std::vector<std::string>& face_filenames);

        /// @brief Activa un skybox existente para que se renderice en la escena.
        /// @param name Nombre del skybox a activar. Si no existe, no se activa ninguno.
        void set_active(const std::string& name);

        /// @brief Devuelve el skybox activo actual.
        /// @return Puntero al Skybox activo, o nullptr si no hay ninguno.
        const Skybox* get_active() const;

        // Prohibir copia/movimiento
        Skybox_Manager(const Skybox_Manager&) = delete;
        Skybox_Manager& operator=(const Skybox_Manager&) = delete;
    };

}