#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/ObjParser.h"
#include "engConfig.h"
#include <doctest/doctest.h>

using namespace eng::obj;
using namespace eng;

TEST_CASE("Parsing vertex")
{
    Vertex expected{-1.3143f, 15.0686f, -1.6458f, 1.0f};

    Vertex actual = strToVertex("-1.3143 15.0686 -1.6458");

    CHECK(expected == actual);
}

TEST_CASE("Parsing normal")
{
    Normal expected{
        -0.6011f,
        0.4519f,
        0.6591f,
    };

    Normal actual = strToNormal("-0.6011 0.4519 0.6591");

    CHECK(expected == actual);
}

TEST_CASE("Parsing texture coord")
{
    SUBCASE("All three")
    {
        TextureCoord expected{
            0.6011f,
            0.4519f,
            0.6591f,
        };
        TextureCoord actual = strToNormal("0.6011 0.4519 0.6591");
        CHECK(expected == actual);
    }
    SUBCASE("Two")
    {
        TextureCoord expected{0.6011f, 0.4519f, 0};
        TextureCoord actual = strToNormal("0.6011 0.4519");
        CHECK(expected == actual);
    }
    SUBCASE("One")
    {
        TextureCoord expected{
            0.6011f,
            0,
            0,
        };
        TextureCoord actual = strToNormal("0.6011");
        CHECK(expected == actual);
    }
}

TEST_CASE("Parse stream completely")
{
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TextureCoord> textures;
    std::vector<Triangle> polygons;

    std::stringstream stream{
        "# comment\nv  -1.3143 15.0686 -1.6458\nvn  "
        "-0.6011 0.4519 0.6591\nvt 0.22 0.34\nf 1/1/1 -1/-1/-1 1/1/-1"};

    std::vector expectedVertices{
        vec::Vec4F{-1.3143f, 15.0686f, -1.6458f, 1.0f}};
    std::vector expectedNormals{vec::Vec3F{-0.6011f, 0.4519f, 0.6591f}};
    std::vector expectedTextures{vec::Vec3F{0.22f, 0.34f, 0.0f}};

    parseStream(stream, vertices, normals, textures, polygons);

    REQUIRE(vertices == expectedVertices);
    REQUIRE(normals == expectedNormals);
    REQUIRE(textures == expectedTextures);


    for (const auto &polygon : polygons) {
        for (const auto &component : polygon) {
            REQUIRE(component.vertexOffset == 0);
            REQUIRE(component.normalOffset == 0);
            REQUIRE(component.textureCoordinatesOffset == 0);
        }
    }
}

TEST_CASE("Parse file completely")
{
    std::ifstream file(parseFileTest_source);
    REQUIRE(file.good());

    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<TextureCoord> textures;
    std::vector<Triangle> polygons;

    parseStream(file, vertices, normals, textures, polygons);

    CHECK(vertices.size() == 24461);
    CHECK(normals.size() == 24460);
    CHECK(textures.size() == 0);
    CHECK(polygons.size() == 2 * 24459);
    std::cout << "With pointers: "
              << vertices.size() * sizeof(*vertices.begin()) +
                     polygons.size() * 4 * sizeof(Vertex *)
              << '\n';
}
