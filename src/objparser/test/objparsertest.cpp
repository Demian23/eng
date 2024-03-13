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

TEST_CASE("Parse stream")
{
    std::vector<Vertex> vertices;
    std::vector<PolygonVertexOnly> polygons;

    std::stringstream stream{"# comment\nv  -1.3143 15.0686 -1.6458\nvn  "
                             "-0.6011 0.4519 0.6591\nf 1//1 -1//-1 1//-1"};

    Vertex expectedVertex{-1.3143f, 15.0686f, -1.6458f, 1.0f};

    parseOnlyVerticesAndPolygons(stream, vertices, polygons);

    CHECK(vertices.size() == 1);
    CHECK(polygons.size() == 1);

    CHECK(vertices[0] == expectedVertex);
    for (auto &&polygon : polygons) {
        for (auto &&vertex : polygon) {
            CHECK(vertex == expectedVertex);
        }
    }
}

TEST_CASE("Parse file")
{
    // TODO from config file
    std::ifstream file(parseFileTest_source);
    REQUIRE(file.good());

    std::vector<Vertex> vertices;
    std::vector<PolygonVertexOnly> polygons;

    auto start = std::chrono::high_resolution_clock::now();
    parseOnlyVerticesAndPolygons(file, vertices, polygons);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    CHECK(vertices.size() == 24461);
    CHECK(polygons.size() == 24459);

    std::vector<Vertex> expectedComponents{
        vertices[0],
        vertices[1],
        vertices[2],
        vertices[3],
    };

    std::cout << vertices.size() * sizeof(Vertex) << ' '
              << polygons.size() * 4 * sizeof(PolygonComponent) << '\n'
              << "Duration(milliseconds): " << duration.count() << std::endl;

    CHECK(polygons.begin()->size() == 4);
    CHECK(std::equal(polygons[0].begin(), polygons[0].end(),
                     expectedComponents.begin()));
}
