#pragma once

#include <istream>
#include <iterator>
#include <string_view>
#include "Elements.h"

namespace obj{

constexpr char objectTypeDelimeter = ' ';

using namespace std::literals::string_view_literals;
Vertex strToVertex(std::string_view stringRep);
Normal strToNormal(std::string_view stringRep);


constexpr std::array typesOfObjectToParse {"v"sv, "vn"sv};
enum class Object {Nothing, Vertex, Normal};

inline Object strToType(std::string_view str){
    if(typesOfObjectToParse[0] == str) return Object::Vertex;
    if(typesOfObjectToParse[1] == str) return Object::Normal;
    return Object::Nothing; 
}

template<typename VertexContainer, typename NormalContainer>
void parse(std::istream& stream, VertexContainer& vcont, NormalContainer& ncont)
{
    auto vertexOutput = std::back_inserter(vcont);
    auto normalOutput = std::back_inserter(ncont);
    auto vertexParser = [&](std::string_view stringRep){vertexOutput = strToVertex(stringRep);};
    auto normalParser = [&](std::string_view stringRep){normalOutput= strToNormal(stringRep);};

    for(std::string line{}; stream.good() && std::getline(stream, line);){

        auto delimeterPosition = line.find(objectTypeDelimeter);
        std::string_view element {line.data(), delimeterPosition};
        std::string_view strRep {line.data() + delimeterPosition + 1, line.size() - delimeterPosition - 1};
        
        switch(strToType(element)){
            case Object::Vertex:
                vertexParser(strRep);
                break;
            case Object::Normal:
                normalParser(strRep);
                break;
            default:
                break;
        }
    }


}

}
