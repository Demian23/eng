#pragma once

#include "../../base/src/Elements.h"
#include "../../base/src/PolygonVertexOnly.h"
#include "ParsingFunctions.h"
#include <istream>
#include <iterator>
#include <string_view>

namespace eng::obj {

template <typename VertexContainer, typename PolygonContainer>
    requires std::same_as<typename VertexContainer::value_type, Vertex> &&
             (std::same_as<typename PolygonContainer::value_type,
                           PolygonVertexOnly> ||
              std::same_as<typename PolygonContainer::value_type,
                           TriangleVertexOnly> ||
              std::same_as<typename PolygonContainer::value_type,
                           QuadVertexOnly>)
void parseOnlyVerticesAndPolygons(std::istream &stream,
                                  VertexContainer &vertices,
                                  PolygonContainer &polygons)
{
    auto vertexInserter = std::back_inserter(vertices);
    auto polygonInserter = std::back_inserter(polygons);

    auto vertexParser = [&](std::string_view stringRep) {
        vertexInserter = strToVertex(stringRep);
    };

    auto polygonParser = [&](std::string_view stringRep) {
        polygonInserter = makePolygon<typename PolygonContainer::value_type>(
            stringRep, vertices);
    };

    for (std::string line{}; stream.good() && std::getline(stream, line);) {
        if (line.ends_with('\r'))
            line.pop_back();
        auto delimiterPosition = line.find(objectTypeDelimiter);
        std::string_view element{line.data(), delimiterPosition};

        switch (strToType(element)) {
        case Object::Vertex:
            vertexParser({line.data() + delimiterPosition + 1,
                          line.size() - delimiterPosition - 1});
            break;
        case Object::Polygon:
            polygonParser({line.data() + delimiterPosition + 1,
                           line.size() - delimiterPosition - 1});
            break;
        default:
            break;
        }
    }
}

} // namespace eng::obj
