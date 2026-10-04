#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <AccCtrl.h>
#include <Proxy.hpp>
#include <Iterator.hpp>

namespace open_gl_engine
{
    template <typename Value_Type>
    class Sparse_Array
    {
    public:
        using Value_Type_t = Value_Type;
        using Optional_Value_Type = std::optional<Value_Type>;

    private:
        using Segment_Type = Segment<Value_Type>;
        using Segment_Pointer = std::unique_ptr<Segment_Type>;

        class Collection
        {
            friend class Sparse_Array_Iterator<Value_Type>;
            std::vector<Segment_Pointer> segments;

        public:
            Proxy<Value_Type> operator[](size_t element_index)
            {
                size_t segment_index = element_index >> Segment_Type::shift;
                if (segment_index >= segments.size())
                    segments.resize(segment_index + 1);

                auto& segment = segments[segment_index];
                if (!segment)
                    segment = std::make_unique<Segment_Type>();

                return Proxy<Value_Type>{ segment.get(), element_index& Segment_Type::mask };
            }

            bool has_element(size_t element_index) const
            {
                size_t segment_index = element_index >> Segment_Type::shift;
                if (segment_index >= segments.size()) return false;
                const auto& segment = segments[segment_index];
                if (!segment) return false;
                size_t local_idx = element_index & Segment_Type::mask;
                return (segment->bitmap & (uint64_t(1) << local_idx)) != 0;
            }

            // Acceso controlado al vector de segmentos
            std::vector<Segment_Pointer>* get_segments_ptr() { return &segments; }
            const std::vector<Segment_Pointer>* get_segments_ptr() const { return &segments; }
        };

        using Iterator = Sparse_Array_Iterator<Value_Type>;
        Collection collection;

    public:
        Proxy<Value_Type> operator[](size_t index)
        {
            return collection[index];
        }

        bool has_value(size_t index) const
        {
            return collection.has_element(index);
        }

        Iterator begin()
        {
            return Iterator(collection.get_segments_ptr(), 0);
        }

        Iterator end()
        {
            return Iterator(collection.get_segments_ptr(), Iterator::SENTINEL);
        }
    };
}