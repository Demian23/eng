#include "ObjParser.h"
#include "Elements.h"
#include <cstdlib>

namespace eng::obj {

vec::ThreeDimensionalVector parseVector(std::string_view stringRep,
                                        char **lastPos)
{
    vec::ThreeDimensionalVector result{};
    char *nextPos{};
    result[0] = std::strtof(stringRep.data(), lastPos);
    result[1] = std::strtof(*lastPos, &nextPos);
    result[2] = std::strtof(nextPos, lastPos);
    return result;
}

Vertex strToVertex(std::string_view stringRep)
{
    // auto [ptr, err] = std::from_chars(stringRep.data(), stringRep.data() +
    // stringRep.size(), newVertex.x, 10);
    //  TODO think about errors
    char *lastPos{};
    auto v = parseVector(stringRep, &lastPos);
    Vertex newVertex{v[0], v[1], v[2], 1.0f};
    if (lastPos != stringRep.end())
        newVertex[3] = std::strtof(lastPos, nullptr);
    return newVertex;
}

Normal strToNormal(std::string_view stringRep)
{
    char *p;
    return parseVector(stringRep, &p);
}

std::vector<PolygonIndexes>
strToPolygonIndexesVector(std::string_view stringRep)
{
    std::vector<PolygonIndexes> indexesVector;
    char *lastPos = const_cast<char *>(stringRep.data());
    long vertexIndex{}, normalIndex{};
    while ((vertexIndex = std::strtol(lastPos, &lastPos, 10)) != 0) {
        if (*lastPos == '/') {
            lastPos++;
            while (*(lastPos++) != '/')
                ;
            normalIndex = std::strtol(lastPos, &lastPos, 10);
        }
        indexesVector.emplace_back(vertexIndex, normalIndex);
    }
    return indexesVector;
}

std::vector<numeric> strToVerticesIndexes(std::string_view stringRep)
{
    std::vector<numeric> indexesVector;
    char *lastPos = const_cast<char *>(stringRep.data());
    long vertexIndex{};
    while ((vertexIndex = std::strtol(lastPos, &lastPos, 10)) != 0) {
        if (*lastPos == '/') {
            while (*(lastPos++) != ' ')
                ;
        }
        indexesVector.emplace_back(vertexIndex);
    }
    return indexesVector;
}

} // namespace eng::obj
