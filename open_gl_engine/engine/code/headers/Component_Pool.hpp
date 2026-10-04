#pragma once

#include <vector>
#include <optional>
#include <Id.hpp>
#include <Component.hpp>
#include <Sparse_Array.hpp>

namespace open_gl_engine
{
    /// @brief Interfaz base no genérica para cualquier pool de componentes.
    /// Permite consultar existencia y eliminar componentes sin conocer el tipo concreto.
    class IComponentPool
    {
    public:
        virtual ~IComponentPool() = default;

        /// @brief Comprueba si una entidad tiene componente en este pool.
        /// @param entity Identificador de la entidad.
        /// @return true si la entidad posee el componente.
        virtual bool has_component(Id entity) const = 0;

        /// @brief Elimina el componente de la entidad en este pool.
        /// @param entity Identificador de la entidad.
        virtual void remove_component(Id entity) = 0;
    };

    /// @brief Pool tipado que almacena componentes de un tipo concreto en un Sparse_Array.
    /// Permite añadir, obtener, listar y eliminar componentes de forma eficiente.
    /// @tparam COMPONENT_TYPE Tipo de componente que almacena el pool.
    template<typename COMPONENT_TYPE>
    class ComponentPool : public IComponentPool
    {
        using Component_Type = COMPONENT_TYPE;
        using Pointer_Component_Type = COMPONENT_TYPE*;

    private:
        Sparse_Array<Component_Type> components; ///< Array disperso indexado por ID de entidad.

    public:
        /// @brief Añade o reemplaza un componente para una entidad.
        /// @param id_entity Identificador de la entidad.
        /// @param component Instancia del componente a almacenar (movida).
        /// @return Referencia estable al componente almacenado.
        Component_Type& add_component(Id id_entity, Component_Type&& component)
        {
            // Mueve el componente al slot correspondiente de la entidad
            components[id_entity] = std::move(component);
            // Devuelve referencia directa al valor almacenado
            return components[id_entity].value();
        }

        /// @brief Obtiene el componente de una entidad, si existe.
        /// @param id_entity Identificador de la entidad.
        /// @return optional con un reference_wrapper al componente, o nullopt si no existe.
        std::optional<std::reference_wrapper<Component_Type>> get_component(Id id_entity)
        {
            // Si el slot tiene valor, devuelve un reference_wrapper
            if (components[id_entity].has_value())
                return std::ref(components[id_entity].value());
            else
                return std::nullopt;
        }

        /// @brief Devuelve punteros a todos los componentes activos en el pool.
        /// @return Vector de punteros a los componentes almacenados.
        std::vector<Pointer_Component_Type> get_all_components()
        {
            std::vector<Pointer_Component_Type> result;
            // Itera solo sobre los slots ocupados del sparse array
            for (auto& comp : components)
                result.push_back(&comp);
            return result;
        }

        /// @brief Comprueba si una entidad tiene componente en este pool.
        /// @param id_entity Identificador de la entidad.
        /// @return true si existe el componente.
        bool has_component(Id id_entity) const
        {
            // Consulta directa al sparse array (O(1))
            return components.has_value(id_entity);
        }

        /// @brief Elimina el componente de una entidad del pool.
        /// @param entity Identificador de la entidad.
        void remove_component(Id entity)
        {
            // Asigna nullopt al slot para destruir el objeto y liberar el indicador
            components[entity] = std::nullopt;
        }
    };
}