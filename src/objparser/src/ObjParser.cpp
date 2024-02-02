#include <cstdlib>
#include "ObjParser.h"
#include "Elements.h"

namespace obj{

ThreeComponentVector parseVector(std::string_view stringRep, char** lastPos)
{
    ThreeComponentVector vec;
    char *nextPos {};
    vec.x = std::strtof(stringRep.data(), lastPos);
    vec.y = std::strtof(*lastPos, &nextPos);
    vec.z = std::strtof(nextPos, lastPos);
    return vec;
}

Vertex strToVertex(std::string_view stringRep)
{
    //auto [ptr, err] = std::from_chars(stringRep.data(), stringRep.data() + stringRep.size(), newVertex.x, 10);
    // TODO think about errors
    char* lastPos {};
    auto v = parseVector(stringRep, &lastPos);
    Vertex newVertex{.x = v.x, .y = v.y, .z = v.z, .w = 0};
    if(lastPos != stringRep.end())
        newVertex.w = std::strtof(lastPos, nullptr);
    return newVertex;
}

Normal strToNormal(std::string_view stringRep){char* p; return parseVector(stringRep, &p);}

};
