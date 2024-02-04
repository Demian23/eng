#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/ObjParser.h"
#include <doctest.h>

using namespace eng::obj;

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

TEST_CASE("Parse stream")
{
    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<Polygon> polygons;

    std::stringstream stream{"# comment\nv  -1.3143 15.0686 -1.6458\nvn  "
                             "-0.6011 0.4519 0.6591\nf 1//1 -1//-1 1//-1"};

    Vertex expectedVertex{-1.3143f, 15.0686f, -1.6458f, 1.0f};
    Normal expectedNormal{-0.6011f, 0.4519f, 0.6591f};

    parse(stream, vertices, normals, polygons);

    CHECK(vertices.size() == 1);
    CHECK(normals.size() == 1);
    CHECK(polygons.size() == 1);

    CHECK(vertices[0] == expectedVertex);
    CHECK(normals[0] == expectedNormal);
    for (auto &&polygon : polygons) {
        for (auto &&faceComponent : polygon.f) {
            CHECK(faceComponent.vertex == expectedVertex);
            CHECK(faceComponent.normal.value_or(Normal{}) == expectedNormal);
        }
    }
}

TEST_CASE("Parse file")
{
    // TODO from config file
    std::ifstream file("/Users/egor/Downloads/FinalBaseMesh.obj");
    REQUIRE(file.good());

    std::vector<Vertex> vertices;
    std::vector<Normal> normals;
    std::vector<Polygon> polygons;

    auto start = std::chrono::high_resolution_clock::now();
    parse(file, vertices, normals, polygons);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    CHECK(vertices.size() == 24461);
    CHECK(normals.size() == 24460);
    CHECK(polygons.size() == 24459);

    std::vector<PolygonComponent> expectedComponents{
        {vertices[0], normals[0]},
        {vertices[1], normals[1]},
        {vertices[2], normals[2]},
        {vertices[3], normals[3]},
    };

    std::cout << vertices.size() * sizeof(Vertex) << ' '
              << normals.size() * sizeof(Normal) << ' '
              << polygons.size() * 4 * sizeof(PolygonComponent) << '\n'
              << "Duration(milliseconds): " << duration.count() << std::endl;

    CHECK(polygons.rbegin()->f.size() == 4);
    for (unsigned i = 0; i < polygons.begin()->f.size(); i++) {
        CHECK(polygons.begin()->f[i].normal.value() ==
              expectedComponents[i].normal.value());
        CHECK(polygons.begin()->f[i].vertex == expectedComponents[i].vertex);
    }
}
