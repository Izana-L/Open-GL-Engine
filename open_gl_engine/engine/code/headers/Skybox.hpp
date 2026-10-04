#pragma once

#include <glm.hpp>

namespace open_gl_engine
{
    struct Mesh;
    class Cubemap;

    /// @brief Renderiza un skybox cúbico utilizando un cubemap.
    /// Se apoya en una malla de cubo ya creada y un shader especializado.
    class Skybox
    {
    private:
        const Mesh* cube_mesh;   ///< Malla del cubo (obtenida del Mesh_Directory, no poseída).
        const Cubemap* cubemap;  ///< Textura de cubemap asociada (no poseída).
        unsigned int program_id; ///< Programa de shader para el skybox.

        unsigned int loc_model_view; ///< Localización del uniform model_view_matrix.
        unsigned int loc_projection; ///< Localización del uniform projection_matrix.
        unsigned int loc_sampler;    ///< Localización del uniform sampler (Cubemap).

    public:
        /// @brief Constructor. Asigna el shader y el cubemap, y carga la malla del cubo.
        /// @param shader_program ID del programa de shader ya compilado.
        /// @param cubemap Puntero al cubemap a utilizar.
        Skybox(unsigned int shader_program, const Cubemap* cubemap);

        ~Skybox() = default;

        // No copiable
        Skybox(const Skybox&) = delete;
        Skybox& operator=(const Skybox&) = delete;

        /// @brief Dibuja el skybox usando las matrices de vista y proyección.
        /// @param view Matriz de vista actual (se elimina la traslación).
        /// @param projection Matriz de proyección.
        void render(const glm::mat4& view, const glm::mat4& projection) const;

        /// @brief Devuelve el ID del programa de shader.
        unsigned int get_program() const { return program_id; }
    };
}