#include "ParsingFunctions.h"
#include "../../base/src/Elements.h"
#include <cstdlib>
#include <istream>
#include <string_view>

namespace eng::obj {

uint32_t getPolygonSize(std::istream &stream)
{
    for (std::string line{}; stream.good() && std::getline(stream, line);) {
        if (line.ends_with('\r'))
            line.pop_back();
        auto delimiterPosition = line.find(objectTypeDelimiter);
        std::string_view element{line.data(), delimiterPosition};
        if (strToType(element) == Object::Polygon) {
            return static_cast<uint32_t>(
                strToVerticesIndexes({line.data() + delimiterPosition + 1,
                                      line.size() - delimiterPosition - 1})
                    .size());
        }
    }
    return 0;
}

template <size_t size>
auto parseFloatString(std::string_view stringRep, char **lastPos)
{
    std::array<eng::floating, size> result{};
    char *nextPos = const_cast<char *>(stringRep.data());
    for (unsigned i = 0; i < size; i++) {
        if (i % 2) {
            result[i] = std::strtof(*lastPos, &nextPos);
        } else {
            result[i] = std::strtof(nextPos, lastPos);
        }
    }
    return result;
}

Vertex strToVertex(std::string_view stringRep)
{
    char *lastPos = nullptr;
    auto v = parseFloatString<3>(stringRep, &lastPos);
    Vertex newVertex{v[0], v[1], v[2], 1.0f};
    if (lastPos != stringRep.end())
        newVertex[3] = std::strtof(lastPos, nullptr);
    return newVertex;
}

Normal strToNormal(std::string_view stringRep)
{
    char *p;
    auto v = parseFloatString<3>(stringRep, &p);
    return {v[0], v[1], v[2]};
}

TextureCoord strToTextureCoord(std::string_view stringRep)
{
    TextureCoord result{};
    char *lastPos = nullptr, *nextPos = const_cast<char *>(stringRep.data());
    result[0] = std::strtof(nextPos, &lastPos);
    if (lastPos != stringRep.end()) {
        result[1] = std::strtof(lastPos, &nextPos);
        if (nextPos != stringRep.end()) {
            result[2] = std::strtof(nextPos, &lastPos);
        }
    }
    return result;
}

std::vector<PolygonIndexes>
strToPolygonIndexesVector(std::string_view stringRep)
{
    std::vector<PolygonIndexes> indexesVector;
    char *lastPos = const_cast<char *>(stringRep.data());
    long vertexIndex{}, normalIndex{}, textureCoordinatesIndex{};
    while ((vertexIndex = std::strtoll(lastPos, &lastPos, 10)) != 0) {
        if (*lastPos == '/') {
            lastPos++;
            textureCoordinatesIndex = std::strtoll(lastPos, &lastPos, 10);
            if (*lastPos == '/') {
                lastPos++;
                normalIndex = std::strtoll(lastPos, &lastPos, 10);
            }
        }
        indexesVector.emplace_back(vertexIndex, normalIndex,
                                   textureCoordinatesIndex);
    }
    return indexesVector;
}

std::vector<integral> strToVerticesIndexes(std::string_view stringRep)
{
    std::vector<integral> indexesVector;
    char *lastPos = const_cast<char *>(stringRep.data());
    long vertexIndex{};
    auto strStart = stringRep.data();
    auto strSize = stringRep.size();
    while ((vertexIndex = std::strtol(lastPos, &lastPos, 10)) != 0) {
        if (*lastPos == '/') {
            while ((static_cast<decltype(strSize)>(lastPos - strStart) <
                    strSize) &&
                   *(lastPos++) != ' ')
                ;
        }
        indexesVector.emplace_back(vertexIndex);
    }
    return indexesVector;
}
} // namespace eng::obj
