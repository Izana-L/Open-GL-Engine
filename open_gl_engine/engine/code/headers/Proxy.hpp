#pragma once

#include <new>          
#include <optional>
#include <utility>  
#include "Segment.hpp"

namespace open_gl_engine
{
    /// @brief Proxy opaco sobre un slot dentro de un Segment de un Sparse_Array.
    /// Proporciona acceso de lectura/escritura transparente al valor almacenado,
    /// así como la posibilidad de asignar std::nullopt o std::optional.
    /// @tparam Value_Type Tipo de valor que reside en el segmento.
    template <typename Value_Type>
    class Proxy
    {
        using Segment_Type = Segment<Value_Type>;

        Segment_Type* segment; ///< Puntero al segmento donde reside el dato.
        size_t        index;   ///< Índice local dentro del segmento (0..63 en segmento de 64 slots).

    public:
        /// @brief Construye un proxy asociado a un slot concreto.
        /// @param seg Puntero al segmento propietario.
        /// @param idx Índice del slot.
        Proxy(Segment_Type* seg, size_t idx) : segment{ seg }, index{ idx } {}

        /// @brief Conversión implícita a std::optional. Devuelve una copia del valor si existe.
        operator std::optional<Value_Type>() const
        {
            if (has_value())
            {
                // Obtener puntero al dato y devolver copia
                auto* ptr = reinterpret_cast<const Value_Type*>(
                    &segment->storage[index * sizeof(Value_Type)]);
                return *ptr;
            }
            return std::nullopt;
        }

        /// @brief Acceso directo al valor (lanza excepción si no existe).
        Value_Type& value()
        {
            if (!has_value())
                throw std::bad_optional_access();   // o usa assert
            return *reinterpret_cast<Value_Type*>(&segment->storage[index * sizeof(Value_Type)]);
        }

        /// @brief Versión constante de value().
        const Value_Type& value() const
        {
            if (!has_value())
                throw std::bad_optional_access();
            return *reinterpret_cast<const Value_Type*>(&segment->storage[index * sizeof(Value_Type)]);
        }

        /// @brief Desreferencia (igual que value()).
        Value_Type& operator*() { return value(); }
        const Value_Type& operator*() const { return value(); }
        Value_Type* operator->() { return &value(); }
        const Value_Type* operator->() const { return &value(); }

        /// @brief Asignación desde una copia de Value_Type.
        Proxy& operator=(const Value_Type& value)
        {
            if (has_value())
            {
                // Sobrescribe el valor existente
                auto* ptr = reinterpret_cast<Value_Type*>(
                    &segment->storage[index * sizeof(Value_Type)]);
                *ptr = value;
            }
            else
            {
                // Construye un nuevo objeto en el slot y marca ocupado
                new (&segment->storage[index * sizeof(Value_Type)]) Value_Type(value);
                segment->bitmap |= (uint64_t(1) << index);
            }
            return *this;
        }

        /// @brief Asignación desde un rvalue de Value_Type (movimiento).
        Proxy& operator=(Value_Type&& value)
        {
            if (has_value())
            {
                auto* ptr = reinterpret_cast<Value_Type*>(
                    &segment->storage[index * sizeof(Value_Type)]);
                *ptr = std::move(value);
            }
            else
            {
                // Construye con move y marca ocupado
                new (&segment->storage[index * sizeof(Value_Type)]) Value_Type(std::move(value));
                segment->bitmap |= (uint64_t(1) << index);
            }
            return *this;
        }

        /// @brief Asigna un slot vacío (destruye el objeto si existe).
        Proxy& operator=(std::nullopt_t)
        {
            if (has_value())
            {
                // Destruir el objeto y desmarcar el bit
                auto* ptr = reinterpret_cast<Value_Type*>(
                    &segment->storage[index * sizeof(Value_Type)]);
                ptr->~Value_Type();
                segment->bitmap &= ~(uint64_t(1) << index);
            }
            return *this;
        }

        /// @brief Asigna un std::optional: si tiene valor lo asigna, si no, vacía el slot.
        Proxy& operator=(const std::optional<Value_Type>& opt)
        {
            if (opt)
                return *this = *opt;      // copia del valor interno
            else
                return *this = std::nullopt; // limpia el slot
        }

        /// @brief Comprueba si el slot contiene un valor.
        bool has_value() const
        {
            return segment->bitmap & (uint64_t(1) << index);
        }

        /// @brief Conversión a bool: true si tiene valor.
        explicit operator bool() const { return has_value(); }

        /// @brief Comparación con nullopt.
        bool operator==(std::nullopt_t) const { return !has_value(); }
        bool operator!=(std::nullopt_t) const { return  has_value(); }
    };
}