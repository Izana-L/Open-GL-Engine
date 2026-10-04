#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace open_gl_engine
{
    /// @brief Segmento de un Sparse_Array. Almacena hasta 64 elementos de tipo Value_Type
    /// en un buffer alineado, y utiliza un bitmap de 64 bits para marcar los slots ocupados.
    /// No se puede copiar; sí se puede mover.
    /// @tparam Value_Type Tipo de valor contenido en el segmento.
    template <typename Value_Type>
    struct Segment
    {
        static constexpr size_t size = 64;   ///< Número máximo de elementos por segmento.
        static constexpr size_t shift = 6;   ///< log2(size), usado para dividir índice global.
        static constexpr size_t mask = 63;   ///< size - 1, para obtener índice local.

        uint64_t bitmap = 0; ///< Bitmap de ocupación: bit i = 1 si el slot i contiene un objeto vivo.

        /// @brief Almacenamiento crudo alineado para `size` objetos de tipo Value_Type.
        alignas(Value_Type) std::array<std::byte, sizeof(Value_Type)* size> storage{};

        // ---- Gestión de copia y movimiento ----

        /// @brief Constructor de copia eliminado (la gestión de objetos es manual).
        Segment(const Segment&) = delete;
        /// @brief Operador de asignación de copia eliminado.
        Segment& operator=(const Segment&) = delete;

        /// @brief Constructor de movimiento por defecto.
        Segment(Segment&&) = default;
        /// @brief Operador de asignación de movimiento por defecto.
        Segment& operator=(Segment&&) = default;

        /// @brief Constructor por defecto. Inicializa el bitmap a 0 y el storage vacío.
        Segment() = default;

        /// @brief Destructor. Llama al destructor de cada objeto vivo según el bitmap.
        ~Segment()
        {
            // Recorrer los 64 slots
            for (size_t i = 0; i < size; ++i)
            {
                // Si el bit está encendido, el slot contiene un objeto construido
                if (bitmap & (uint64_t(1) << i))
                {
                    // Obtener puntero al objeto en el storage y destruirlo
                    auto* ptr = reinterpret_cast<Value_Type*>(&storage[i * sizeof(Value_Type)]);
                    ptr->~Value_Type();
                }
            }
        }
    };

} 