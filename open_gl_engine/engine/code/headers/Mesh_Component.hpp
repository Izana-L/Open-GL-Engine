#pragma once
#include <Component.hpp>

namespace open_gl_engine
{
    struct Mesh;   // forward

    /// @brief Componente que asocia una malla (Mesh) a una entidad.
    /// Almacena un puntero al recurso Mesh (no es propietario) para el renderizado.
    struct Mesh_Component : Component
    {
        const Mesh* mesh = nullptr; ///< Puntero a la malla compartida (no propietario).

        /// @brief Constructor que solo asigna la entidad.
        /// @param _entity_id Identificador de la entidad propietaria.
        Mesh_Component(Id& _entity_id) { entity_id = _entity_id; }

        /// @brief Constructor que asigna entidad y malla.
        /// @param _entity_id Identificador de la entidad.
        /// @param _mesh Puntero a la malla a asociar.
        Mesh_Component(Id& _entity_id, const Mesh* _mesh)
        {
            entity_id = _entity_id;
            mesh = _mesh;
        }

        // Copia trivial: solo copia el puntero (el Mesh reside en el Mesh_Manager).
        Mesh_Component(const Mesh_Component&) = default;
        Mesh_Component& operator=(const Mesh_Component&) = default;

        /// @brief Destructor. Pone el puntero a nullptr (no libera la malla).
        ~Mesh_Component() { mesh = nullptr; }
    };
}