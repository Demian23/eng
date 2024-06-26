#pragma once

#include "../../algorithm/src/alg.h"
#include "../../base/src/Elements.h"
#include "ParsingFunctions.h"
#include <istream>
#include <iterator>
#include <string_view>

namespace eng::obj {

template <typename VertexContainer, typename NormalContainer,
          typename TextureCoordContainer, typename PolygonContainer>
    requires std::same_as<typename VertexContainer::value_type, Vertex> &&
             std::same_as<typename PolygonContainer::value_type, Triangle> &&
             std::same_as<typename NormalContainer::value_type, Normal> &&
             std::same_as<typename TextureCoordContainer::value_type,
                          TextureCoord>
void parseStream(std::istream &stream, VertexContainer &vertices,
                 NormalContainer &normals, TextureCoordContainer &textCoord,
                 PolygonContainer &polygons)
{
    auto vertexInserter = std::back_inserter(vertices);
    auto normalInserter = std::back_inserter(normals);
    auto textureInserter = std::back_inserter(textCoord);
    auto polygonInserter = std::back_inserter(polygons);

    auto vertexParser = [&](std::string_view stringRep) {
        vertexInserter = strToVertex(stringRep);
    };

    auto normalParser = [&](std::string_view stringRep) {
        normalInserter = strToNormal(stringRep);
    };
    auto textureParser = [&](std::string_view stringRep) {
        textureInserter = strToTextureCoord(stringRep);
    };

    auto polygonParser = [&](std::string_view stringRep) {
        const auto indexes = obj::strToPolygonIndexesVector(stringRep);
        Polygon newPolygon{indexes.size()};
        auto polygonIterator = newPolygon.begin();
        for (auto [vertexIndex, normalIndex, textureCoordIndex] : indexes) {
            vertexIndex =
                vertexIndex < 0
                    ? static_cast<eng::integral>(
                          vertices.size() +
                          static_cast<size_t>(std::abs(vertexIndex)) - 2)
                    : vertexIndex - 1;
            normalIndex =
                normalIndex == 0 ? PolygonComponent::invalidOffset
                : normalIndex < 0
                    ? static_cast<eng::integral>(
                          normals.size() +
                          static_cast<size_t>(std::abs(normalIndex)) - 2)
                    : normalIndex - 1;
            textureCoordIndex =
                textureCoordIndex == 0 ? PolygonComponent::invalidOffset
                : textureCoordIndex < 0
                    ? static_cast<eng::integral>(
                          textCoord.size() +
                          static_cast<size_t>(std::abs(textureCoordIndex)) - 2)
                    : (textureCoordIndex - 1);

            *polygonIterator = {vertexIndex, normalIndex, textureCoordIndex};
            ++polygonIterator;
        }

        if (newPolygon.size() == 3) {
            polygonInserter = {newPolygon[0], newPolygon[1], newPolygon[2]};
        } else {
            alg::polygonTriangulation(newPolygon.begin(), newPolygon.end(),
                                      polygonInserter);
        }
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
        case Object::Normal:
            normalParser({line.data() + delimiterPosition + 1,
                          line.size() - delimiterPosition - 1});
            break;
        case Object::TextureCoord:
            textureParser({line.data() + delimiterPosition + 1,
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
