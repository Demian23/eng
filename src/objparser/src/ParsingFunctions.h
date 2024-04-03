#pragma once

#include "../../base/src/Elements.h"
#include <string_view>
namespace eng::obj {

constexpr char objectTypeDelimiter = ' ';

using std::literals::string_view_literals::operator""sv;

constexpr std::array typesOfObjectToParse{"v"sv, "vn"sv, "vt"sv, "f"sv};

enum class Object { Nothing, Vertex, Normal, TextureCoord, Polygon };

inline Object strToType(std::string_view str)
{
    if (typesOfObjectToParse[0] == str)
        return Object::Vertex;
    if (typesOfObjectToParse[1] == str)
        return Object::Normal;
    if (typesOfObjectToParse[2] == str)
        return Object::TextureCoord;
    if (typesOfObjectToParse[3] == str)
        return Object::Polygon;
    return Object::Nothing;
}

Vertex strToVertex(std::string_view stringRep);
Normal strToNormal(std::string_view stringRep);
TextureCoord strToTextureCoord(std::string_view stringRep);

std::vector<PolygonIndexes>
strToPolygonIndexesVector(std::string_view stringRep);

} // namespace eng::obj