#pragma once

#include "../../objparser/src/ParsingFunctions.h"
#include "../../vector/src/DimensionalVector.h"
#include "Elements.h"

namespace eng {

struct PolygonVertexOnly final : std::vector<vec::Vec4F> {};

struct TriangleVertexOnly final : std::array<vec::Vec4F, 3> {};
struct QuadVertexOnly final : std::array<vec::Vec4F, 4> {};

template <typename PolygonType>
PolygonType makePolygon(std::string_view strRep,
                        const std::vector<Vertex> &allVertex)
{
    const std::vector<integral> indexes = obj::strToVerticesIndexes(strRep);
    PolygonType newPolygon{};
    if constexpr (std::is_same_v<PolygonType, PolygonVertexOnly>) {
        newPolygon.resize(indexes.size());
    }
    auto polygonIterator = newPolygon.begin();
    for (auto index : indexes) {
        size_t vectorIndex =
            index < 0
                ? allVertex.size() + static_cast<size_t>(std::abs(index)) - 2
                : static_cast<size_t>(index - 1);
        *polygonIterator = allVertex[vectorIndex];
        ++polygonIterator;
    }
    return newPolygon;
}

} // namespace eng
