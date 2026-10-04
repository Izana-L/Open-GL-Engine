#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <typeindex>
#include <Kernel.hpp>        
#include <Component_Pool.hpp>
#include <Sparse_Array.hpp>
#include <Id_Provider.hpp>
#include <ComponentBits.hpp>

namespace open_gl_engine
{
    class Window;

    /// @brief Contenedor principal de la escena ECS.
    /// Gestiona entidades (IDs), componentes (pools por tipo) y máscaras de bits.
    /// También posee el Kernel, la Window y coordina las tareas del bucle principal.
    class Scene
    {
    private:
        /// @brief Mapa de pools de componentes indexado por type_index.
        /// Almacena un pool único para cada tipo de componente.
        std::unordered_map<std::type_index, std::unique_ptr<IComponentPool>> component_pools;

        /// @brief Máscara de bits de componentes por entidad.
        /// Sparse_Array indexado por ID de entidad; cada valor es un OR de ComponentBit.
        Sparse_Array<uint32_t> entity_masks;

        Window& window;                  ///< Referencia a la ventana de la aplicación.
        Kernel kernel;                   ///< Núcleo del motor (bucle, delta, shaders).
        Id_Provider id_provider;         ///< Generador de IDs únicos para entidades.
        bool post_processing_active = false; ///< Indica si el postprocesado está habilitado.

    public:
        /// @brief Constructor. Recibe la ventana y crea las tareas principales.
        /// @param my_window Referencia a la ventana de SDL.
        Scene(Window& my_window);

        /// @brief Activa/desactiva el postprocesado.
        void set_post_processing_active(bool active) { post_processing_active = active; }
        /// @brief Consulta si el postprocesado está activo.
        bool is_post_processing_active() const { return post_processing_active; }

        /// @brief Obtiene el Kernel de la escena.
        Kernel& get_kernel() { return kernel; }

        /// @brief Obtiene la Window de la escena (no const).
        Window& get_window() { return window; }
        /// @brief Obtiene la Window de la escena (const).
        const Window& get_window() const { return window; }

        /// @brief Crea una nueva entidad sin componentes.
        /// @return ID único de la nueva entidad.
        Id add_entity()
        {
            Id entity_id = id_provider.allocate_id();
            entity_masks[entity_id] = 0;          // entidad sin componentes
            return entity_id;
        }

        /// @brief Obtiene la máscara de bits de componentes de una entidad.
        /// @param entity_id ID de la entidad.
        /// @return Máscara de bits actual o 0 si no existe la entidad.
        uint32_t get_entity_mask(Id entity_id)
        {
            return entity_masks.has_value(entity_id) ? entity_masks[entity_id].value() : 0;
        }

        /// @brief Añade un componente a una entidad.
        /// Construye el componente con los argumentos dados y actualiza la máscara de la entidad.
        /// @tparam COMPONENT_TYPE Tipo del componente.
        /// @tparam Args Argumentos para el constructor del componente.
        /// @param entity_id ID de la entidad.
        /// @param args Argumentos para construir el componente.
        /// @return Referencia al componente recién creado.
        template<typename COMPONENT_TYPE, typename... Args>
        COMPONENT_TYPE& add_component(Id entity_id, Args&&... args)
        {
            // 1. Obtener/crear el pool del tipo adecuado
            auto& pool = get_or_create_pool<COMPONENT_TYPE>();
            // 2. Construir el componente en el pool
            COMPONENT_TYPE& new_comp = pool.add_component(entity_id,
                COMPONENT_TYPE(entity_id, std::forward<Args>(args)...));

            // 3. Encender el bit correspondiente en la máscara
            uint32_t bit = ComponentBitOf<COMPONENT_TYPE>::value;
            uint32_t current_mask = entity_masks[entity_id].value();
            entity_masks[entity_id] = current_mask | bit;

            return new_comp;
        }

        /// @brief Obtiene un componente de una entidad.
        /// @tparam COMPONENT_TYPE Tipo del componente.
        /// @param id_entity ID de la entidad.
        /// @return std::optional con un reference_wrapper al componente, o nullopt si no existe.
        template<typename COMPONENT_TYPE>
        std::optional<std::reference_wrapper<COMPONENT_TYPE>> get_component(Id id_entity)
        {
            auto type = std::type_index(typeid(COMPONENT_TYPE));
            auto it = component_pools.find(type);
            if (it == component_pools.end()) return std::nullopt;

            auto* pool = static_cast<ComponentPool<COMPONENT_TYPE>*>(it->second.get());
            return pool->get_component(id_entity);
        }

        /// @brief Obtiene un vector de punteros a todos los componentes de un tipo.
        /// @tparam COMPONENT_TYPE Tipo de componente.
        /// @return Vector de punteros a componentes activos.
        template<typename COMPONENT_TYPE>
        std::vector<COMPONENT_TYPE*> get_all_components_type() {
            auto type = std::type_index(typeid(COMPONENT_TYPE));
            auto it = component_pools.find(type);
            if (it != component_pools.end()) {
                auto* pool = static_cast<ComponentPool<COMPONENT_TYPE>*>(it->second.get());
                return pool->get_all_components();
            }
            return {};
        }

        /// @brief Inicia el bucle principal de la escena.
        void run()
        {
            kernel.execute(window);
        }

        /// @brief Detiene el bucle principal (llama a kernel.stop()).
        void stop()
        {
            kernel.stop();
        }

    private:
        /// @brief Obtiene el pool de un tipo de componente, o lo crea si no existe.
        /// @tparam COMPONENT_TYPE Tipo de componente.
        /// @return Referencia al ComponentPool correspondiente.
        template<typename COMPONENT_TYPE>
        ComponentPool<COMPONENT_TYPE>& get_or_create_pool() {
            auto type = std::type_index(typeid(COMPONENT_TYPE));
            auto it = component_pools.find(type);
            if (it == component_pools.end()) {
                auto pool = std::make_unique<ComponentPool<COMPONENT_TYPE>>();
                auto& ref = *pool;
                component_pools[type] = std::move(pool);
                return ref;
            }
            return *static_cast<ComponentPool<COMPONENT_TYPE>*>(it->second.get());
        }
    };
}