#include <sstream>
#include <vector>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include "../src/ObjParser.h"

bool operator==(const obj::Vertex& a, const obj::Vertex& b){return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;}
bool operator==(const obj::Normal& a, const obj::Normal& b){return a.x == b.x && a.y == b.y && a.z == b.z;}

TEST_CASE("Parsing vertex"){
    obj::Vertex expected {-1.3143f, 15.0686f, -1.6458f, 0.0f};
    
    obj::Vertex actual = obj::strToVertex("-1.3143 15.0686 -1.6458");

    CHECK(expected == actual);
}

TEST_CASE("Parsing normal"){
    obj::Normal expected {-0.6011f, 0.4519f, 0.6591f, };
    
    obj::Normal actual = obj::strToNormal("-0.6011 0.4519 0.6591");

    CHECK(expected == actual);
}

TEST_CASE("Parse stream"){
    std::vector<obj::Vertex> vertices;
    std::vector<obj::Normal> normals;

    std::stringstream stream
        {"# comment\nv  -1.3143 15.0686 -1.6458\nvn  -0.6011 0.4519 0.6591"};

    obj::Vertex expectedVertex {-1.3143f, 15.0686f, -1.6458f, 0.0f};
    obj::Normal expectedNormal {-0.6011f, 0.4519f, 0.6591f, };

    obj::parse(stream, vertices, normals);

    CHECK(vertices.size() == 1);
    CHECK(normals.size() == 1);

    CHECK(vertices[0] == expectedVertex);
    CHECK(normals[0] == expectedNormal);
}

