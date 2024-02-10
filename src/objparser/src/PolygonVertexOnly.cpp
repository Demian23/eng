#include "PolygonVertexOnly.h"
#include "ObjParser.h"

namespace eng::obj {

PolygonVertexOnly
PolygonVertexOnly::makePolygon(std::string_view strRep,
                               const std::vector<Vertex> &allVertex)
{
    const std::vector<numeric> indexes = strToVerticesIndexes(strRep);
    PolygonVertexOnly newPolygon{indexes.size()};
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
