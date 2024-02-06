#pragma once

#include "Elements.h"
#include <istream>
#include <iterator>
#include <string_view>

namespace eng::obj {

constexpr char objectTypeDelimiter = ' ';

using namespace std::literals::string_view_literals;

Vertex strToVertex(std::string_view stringRep);
Normal strToNormal(std::string_view stringRep);

std::vector<PolygonIndexes>
strToPolygonIndexesVector(std::string_view stringRep);

template <typename VertexContainer, typename NormalContainer>
Polygon makePolygon(const std::vector<PolygonIndexes> &indexes,
                    VertexContainer &vcont, NormalContainer &ncont)
{
    Polygon newPolygon{indexes.size()};
    auto polygonIterator = newPolygon.f.begin();
    for (auto &&index : indexes) {
        PolygonComponent newComponent{};
        if (index.vertexIndex != 0) {
            if (index.vertexIndex > 0)
                newComponent.vertex = *(vcont.begin() + index.vertexIndex - 1);
            else
                newComponent.vertex =
                    *(vcont.rbegin() + std::abs(index.vertexIndex) - 1);
        }
        if (index.normalIndex != 0) {
            if (index.normalIndex > 0)
                newComponent.normal = *(ncont.begin() + index.normalIndex - 1);
            else
                newComponent.normal =
                    *(ncont.rbegin() + std::abs(index.normalIndex) - 1);
        }
        *polygonIterator = newComponent;
        ++polygonIterator;
    }
    return newPolygon;
}

constexpr std::array typesOfObjectToParse{"v"sv, "vn"sv, "f"sv};
enum class Object { Nothing, Vertex, Normal, Polygon };

inline Object strToType(std::string_view str)
{
    if (typesOfObjectToParse[0] == str)
        return Object::Vertex;
    if (typesOfObjectToParse[1] == str)
        return Object::Normal;
    if (typesOfObjectToParse[2] == str)
        return Object::Polygon;
    return Object::Nothing;
}

template <typename VertexContainer, typename NormalContainer,
          typename FaceContainer>
void parse(std::istream &stream, VertexContainer &vcont, NormalContainer &ncont,
           FaceContainer &fcont)
{
    auto vertexOutput = std::back_inserter(vcont);
    auto normalOutput = std::back_inserter(ncont);
    auto faceOutput = std::back_inserter(fcont);

    auto vertexParser = [&](std::string_view stringRep) {
        vertexOutput = strToVertex(stringRep);
    };
    auto normalParser = [&](std::string_view stringRep) {
        normalOutput = strToNormal(stringRep);
    };
    auto polygonParser = [&](std::string_view stringRep) {
        faceOutput =
            makePolygon(strToPolygonIndexesVector(stringRep), vcont, ncont);
    };

    for (std::string line{}; stream.good() && std::getline(stream, line);) {
        if(line.ends_with('\r')) line.pop_back();
        auto delimeterPosition = line.find(objectTypeDelimiter);
        std::string_view element{line.data(), delimeterPosition};
        std::string_view strRep{line.data() + delimeterPosition + 1,
                                line.size() - delimeterPosition - 1};

        switch (strToType(element)) {
        case Object::Vertex:
            vertexParser(strRep);
            break;
        case Object::Normal:
            normalParser(strRep);
            break;
        case Object::Polygon:
            polygonParser(strRep);
            break;
        default:
            break;
        }
    }
}

} // namespace eng::obj
