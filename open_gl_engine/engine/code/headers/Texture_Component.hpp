// Texture_Component.hpp
#pragma once
#include <Component.hpp>

namespace open_gl_engine
{
    class Texture;   // forward declaration

    /// @brief Componente que asocia una textura a una entidad.
    /// Almacena un puntero no propietario a una Texture (gestionada por Texture_Manager).
    struct Texture_Component : Component
    {
        const Texture* texture = nullptr; ///< Puntero a la textura asociada (no propietario).

        /// @brief Constructor que solo asigna la entidad (textura nula).
        /// @param _entity_id Identificador de la entidad propietaria.
        Texture_Component(Id& _entity_id) { entity_id = _entity_id; }

        /// @brief Constructor que asigna la entidad y la textura.
        /// @param _entity_id Identificador de la entidad.
        /// @param _texture Puntero a la textura que se asociará.
        Texture_Component(Id& _entity_id, const Texture* _texture)
            : texture(_texture)
        {
            entity_id = _entity_id;
        }
    };
}