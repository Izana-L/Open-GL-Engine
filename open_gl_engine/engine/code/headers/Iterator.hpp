#pragma once

#include <cstddef>   // size_t
#include <memory>    // unique_ptr
#include <vector>
#include "Segment.hpp"

namespace open_gl_engine
{
    /// @brief Iterador que recorre únicamente los slots ocupados de un Sparse_Array.
    /// Se apoya en los segmentos y sus bitmaps para saltar los elementos vacíos.
    /// @tparam Value_Type Tipo de valor almacenado en el array disperso.
    template <typename Value_Type>
    class Sparse_Array_Iterator
    {
    public:
        /// @brief Valor centinela que señala el final de la iteración.
        static constexpr size_t SENTINEL = static_cast<size_t>(-1);

        using Segment_Type = Segment<Value_Type>;
        using Segment_Pointer = std::unique_ptr<Segment_Type>;

    private:
        std::vector<Segment_Pointer>* segments_ptr; ///< Puntero al vector de segmentos del array.
        size_t current_global_index;                ///< Índice global del slot actual (o SENTINEL si es fin).

    public:
        /// @brief Constructor que posiciona el iterador en el primer slot ocupado desde `idx`.
        /// @param segs Puntero al vector de segmentos.
        /// @param idx  Índice global de inicio (normalmente 0 para begin()).
        Sparse_Array_Iterator(std::vector<Segment_Pointer>* segs, size_t idx)
            : segments_ptr{ segs }, current_global_index{ idx }
        {
            // Si no es centinela, salta los slots vacíos iniciales
            if (current_global_index != SENTINEL)
                advance_to_next_occupied();
        }

        /// @brief Pre-incremento: avanza al siguiente slot ocupado.
        /// @return Referencia al propio iterador.
        Sparse_Array_Iterator& operator++()
        {
            if (current_global_index != SENTINEL)
            {
                ++current_global_index;
                advance_to_next_occupied(); // Salta vacíos
            }
            return *this;
        }

        /// @brief Accede al valor almacenado en el slot actual.
        /// @return Referencia al valor (Value_Type&).
        Value_Type& operator*() const
        {
            // Calcula segmento y posición local usando los bits de shift y mask
            size_t seg_idx = current_global_index >> Segment_Type::shift;
            auto& segment = (*segments_ptr)[seg_idx];
            size_t local_idx = current_global_index & Segment_Type::mask;

            // Construye la referencia a partir del almacenamiento del segmento
            return *reinterpret_cast<Value_Type*>(
                &segment->storage[local_idx * sizeof(Value_Type)]);
        }

        /// @brief Compara dos iteradores. Dos iteradores son iguales si recorren el mismo array y están en la misma posición.
        bool operator==(const Sparse_Array_Iterator& other) const
        {
            return segments_ptr == other.segments_ptr &&
                current_global_index == other.current_global_index;
        }

        /// @brief Desigualdad: complemento de operator==.
        bool operator!=(const Sparse_Array_Iterator& other) const
        {
            return !(*this == other);
        }

    private:
        /// @brief Avanza el índice global hasta encontrar un slot ocupado o el fin del array.
        void advance_to_next_occupied()
        {
            while (true)
            {
                size_t seg_idx = current_global_index >> Segment_Type::shift;

                // Si el segmento está fuera del vector, fin
                if (seg_idx >= segments_ptr->size())
                {
                    current_global_index = SENTINEL;
                    break;
                }

                auto& segment = (*segments_ptr)[seg_idx];
                size_t local_idx = current_global_index & Segment_Type::mask;

                // Si el segmento existe y su bitmap marca ocupado, se detiene
                if (segment && (segment->bitmap & (uint64_t(1) << local_idx)))
                    break;

                // Slot vacío: pasa al siguiente índice global
                ++current_global_index;
            }
        }
    };

} 