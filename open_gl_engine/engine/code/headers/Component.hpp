#pragma once

#include <string>
#include <Id.hpp>

namespace open_gl_engine
{
    /// @brief Clase base de todos los componentes del motor.
    /// Proporciona un identificador de entidad común y una interfaz polimórfica básica.
    struct Component
    {
    protected:
        Id entity_id = INVALID_ID;  ///< Identificador de la entidad a la que pertenece el componente.

        /// @brief Destructor virtual protegido para permitir herencia polimórfica.
        virtual ~Component() = default;

    public:
        /// @brief Obtiene el ID de la entidad propietaria (versión no constante).
        /// @return Identificador de la entidad.
        Id get_entity()
        {
            // Devuelve el ID almacenado.
            return entity_id;
        }

        /// @brief Obtiene el ID de la entidad propietaria (versión constante).
        /// @return Identificador de la entidad.
        Id get_entity() const
        {
            // Devuelve el ID almacenado.
            return entity_id;
        }
    };
}