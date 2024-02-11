#pragma once

#include "Elements.h"
#include "ParsingFunctions.h"

namespace eng::obj {
struct PolygonVertexOnly final : std::vector<Vertex> {
};


struct TriangleVertexOnly final : std::array<Vertex, 3> {
};
struct QuadVertexOnly final : std::array<Vertex, 4> {
};

template<typename PolygonType> PolygonType makePolygon(std::string_view strRep,
            const std::vector<Vertex> &allVertex)
{
    const std::vector<numeric> indexes = strToVerticesIndexes(strRep);
    PolygonType newPolygon{};
    if constexpr(std::is_same_v<PolygonType, PolygonVertexOnly>){
        newPolygon.reserve(indexes.size());
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


} // namespace eng::obj