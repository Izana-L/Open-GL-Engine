#pragma once

#include <cstdint>

namespace open_gl_engine
{
    // Declaraciones adelantadas de los tipos de componente
    struct Transform_Component;
    struct Mesh_Component;
    struct Camera_Component;
    struct Texture_Component;
    struct Movement_Component;

    /// @brief Máscaras de bits para identificar tipos de componente.
    /// Cada bit representa una familia de componentes, permitiendo consultas rápidas.
    enum ComponentBit : uint32_t
    {
        CB_Transform = 1 << 0,  ///< Bit para Transform_Component.
        CB_Mesh = 1 << 1,  ///< Bit para Mesh_Component.
        CB_Camera = 1 << 2,  ///< Bit para Camera_Component.
        CB_Texture = 1 << 3,  ///< Bit para Texture_Component.
        CB_Movement = 1 << 4,  ///< Bit para Movement_Component.
    };

    /// @brief Trait que asocia cada tipo de componente con su máscara de bit.
    /// Útil para obtener el bit correspondiente a partir del tipo en tiempo de compilación.
    /// @tparam T Tipo de componente.
    template<typename T> struct ComponentBitOf;

    /// @brief Especialización para Transform_Component.
    template<> struct ComponentBitOf<Transform_Component> { static constexpr uint32_t value = CB_Transform; };
    /// @brief Especialización para Mesh_Component.
    template<> struct ComponentBitOf<Mesh_Component> { static constexpr uint32_t value = CB_Mesh; };
    /// @brief Especialización para Camera_Component.
    template<> struct ComponentBitOf<Camera_Component> { static constexpr uint32_t value = CB_Camera; };
    /// @brief Especialización para Texture_Component.
    template<> struct ComponentBitOf<Texture_Component> { static constexpr uint32_t value = CB_Texture; };
    /// @brief Especialización para Movement_Component.
    template<> struct ComponentBitOf<Movement_Component> { static constexpr uint32_t value = CB_Movement; };

}