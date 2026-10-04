#pragma once

#include <cstdint>
#include <Id.hpp>

namespace open_gl_engine
{
    class Scene;

    /// @brief Representa una entidad dentro del motor ECS.
    /// Contiene un identificador único, una referencia a la escena a la que pertenece
    /// y una máscara de bits para consultar rápidamente los tipos de componente que posee.
    struct Entity
    {
        Id id;                     ///< Identificador único de la entidad.
        Scene* scene;              ///< Puntero a la escena propietaria.
        uint32_t componentMask = 0; ///< Máscara de bits con los ComponentBit de los componentes asociados.

        /// @brief Constructor que asigna el ID y la escena.
        /// @param my_scene Puntero a la escena que contiene la entidad.
        /// @param my_id Identificador único para la entidad.
        Entity(Scene* my_scene, Id my_id);

        /// @brief Comprueba si la entidad tiene todos los componentes indicados por la máscara.
        /// @param mask Máscara de bits con los componentes a verificar.
        /// @return true si la entidad posee todos los bits especificados.
        bool hasComponents(uint32_t mask) const
        {
            // Aplica la máscara y compara con la propia; devuelve true si coinciden los bits pedidos
            return (componentMask & mask) == mask;
        }

        /// @brief Añade un bit de componente a la máscara de la entidad.
        /// @param bit Máscara de bit del componente (valor ComponentBit).
        void addComponentBit(uint32_t bit)
        {
            // Activa el bit mediante OR
            componentMask |= bit;
        }

        /// @brief Elimina un bit de componente de la máscara de la entidad.
        /// @param bit Máscara de bit del componente a quitar.
        void removeComponentBit(uint32_t bit)
        {
            // Apaga el bit mediante AND con el complemento
            componentMask &= ~bit;
        }
    };
}