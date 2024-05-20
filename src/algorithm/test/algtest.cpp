#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/alg.h"
#include "engConfig.h"
#include <algorithm>
#include <array>
#include <chrono>
#include <doctest/doctest.h>
#include <iostream>
#include <random>

using namespace eng::alg;

struct valueType : public std::pair<long, long> {
    std::string stringRep;
    valueType(long first, long second) : std::pair<long, long>(first, second)
    {
        stringRep =
            std::to_string(this->first) + "," + std::to_string(this->second);
    }
    operator const char *() const { return stringRep.c_str(); }
};

TEST_CASE("Line points generating")
{
    std::vector<valueType> points;
    std::vector<valueType> expected{{0, 3}, {1, 2}, {2, 2}, {3, 1}, {4, 0}};
    line(0, 4, 3, 0, [&points](long first, long second) {
        points.emplace_back(first, second);
    });
    CHECK_EQ(expected, points);
}

TEST_CASE("ImprovedLine points generating")
{
    std::vector<valueType> points;
    std::vector<valueType> expected{{0, 3}, {1, 2}, {2, 2}, {3, 1}, {4, 0}};
    improvedBresenhamLine(0, 4, 3, 0, [&points](long first, long second) {
        points.emplace_back(first, second);
    });
    CHECK_EQ(expected, points);
}
TEST_CASE("DDALine points generating")
{
    std::vector<valueType> points;
    std::vector<valueType> expected{{0, 3}, {1, 2}, {2, 2}, {3, 1}, {4, 0}};
    ddaLine(0.0f, 4.0f, 3.0f, 0.0f, [&points](long first, long second) {
        points.emplace_back(first, second);
    });
    CHECK_EQ(expected, points);
}

TEST_CASE("ImprovedLine in difficult cases")
{
    std::vector<valueType> actual;
    std::vector<valueType> expected;

    line(4, 3, 5, 12, [&expected](long first, long second) {
        expected.emplace_back(first, second);
    });
    improvedBresenhamLine(4, 3, 5, 12, [&actual](long first, long second) {
        actual.emplace_back(first, second);
    });
    CHECK_EQ(expected, actual);

    line(4, 3, 3, 2, [&expected](long first, long second) {
        expected.emplace_back(first, second);
    });
    improvedBresenhamLine(4, 3, 3, 2, [&actual](long first, long second) {
        actual.emplace_back(first, second);
    });
    CHECK_EQ(expected, actual);

    line(0, 9, 3, 3, [&expected](long first, long second) {
        expected.emplace_back(first, second);
    });
    improvedBresenhamLine(0, 9, 3, 3, [&actual](long first, long second) {
        actual.emplace_back(first, second);
    });
    CHECK_EQ(expected, actual);
}

TEST_CASE("Time test for Bresenham's line, improvedLine and ddaLine")
{
    std::random_device device;
    std::mt19937 mt(device());
    std::uniform_int_distribution dist(1, 4096);
    std::uniform_real_distribution realDist(1.0, 4096.0);

    struct Line {
        std::pair<int, int> x0y0;
        std::pair<int, int> x1y1;
    };

    struct DDALine {
        std::pair<double, double> x0y0;
        std::pair<double, double> x1y1;
    };

    constexpr auto sampleSize = timeTestForLines_sampleSize;

    std::vector<DDALine> ddaLineInput{sampleSize};
    std::generate(ddaLineInput.begin(), ddaLineInput.end(), [=]() mutable {
        return DDALine{{realDist(mt), realDist(mt)},
                       {realDist(mt), realDist(mt)}};
    });

    std::vector<Line> improvedLine{sampleSize};
    std::generate(improvedLine.begin(), improvedLine.end(), [=]() mutable {
        return Line{{dist(mt), dist(mt)}, {dist(mt), dist(mt)}};
    });

    std::vector<Line> simpleLine{sampleSize};
    std::generate(simpleLine.begin(), simpleLine.end(), [=]() mutable {
        return Line{{dist(mt), dist(mt)}, {dist(mt), dist(mt)}};
    });

    std::vector<valueType> simpleLineResult{};
    std::vector<valueType> improvedLineResult{};
    std::vector<valueType> ddaLineResult{};

    auto startSimple = std::chrono::high_resolution_clock::now();
    std::for_each(simpleLine.begin(), simpleLine.end(),
                  [out = [&simpleLineResult](long first, long second) {
                      simpleLineResult.emplace_back(first, second);
                  }](auto lineRep) mutable {
                      line(lineRep.x0y0.first, lineRep.x1y1.first,
                           lineRep.x0y0.second, lineRep.x1y1.second, out);
                  });
    auto endSimple = std::chrono::high_resolution_clock::now();
    // sleep, cache?
    auto startImproved = std::chrono::high_resolution_clock::now();
    std::for_each(improvedLine.begin(), improvedLine.end(),
                  [out = [&improvedLineResult](long first, long second) {
                      improvedLineResult.emplace_back(first, second);
                  }](auto lineRep) mutable {
                      improvedBresenhamLine(
                          lineRep.x0y0.first, lineRep.x1y1.first,
                          lineRep.x0y0.second, lineRep.x1y1.second, out);
                  });
    auto endImproved = std::chrono::high_resolution_clock::now();

    auto startDDA = std::chrono::high_resolution_clock::now();
    std::for_each(ddaLineInput.begin(), ddaLineInput.end(),
                  [out = [&ddaLineResult](long first, long second) {
                      ddaLineResult.emplace_back(first, second);
                  }](auto lineRep) mutable {
                      ddaLine(lineRep.x0y0.first, lineRep.x1y1.first,
                              lineRep.x0y0.second, lineRep.x1y1.second, out);
                  });
    auto endDDA = std::chrono::high_resolution_clock::now();

    auto simpleDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endSimple - startSimple);
    auto improvedDuration =
        std::chrono::duration_cast<std::chrono::milliseconds>(endImproved -
                                                              startImproved);
    auto ddaDuration = std::chrono::duration_cast<std::chrono::milliseconds>(
        endDDA - startDDA);

    auto minDuration =
        std::min({simpleDuration.count(), improvedDuration.count(),
                  ddaDuration.count()});
    std::cout << ((minDuration == simpleDuration.count())
                      ? "Bresenham"
                      : ((minDuration == improvedDuration.count())
                             ? "Improved Bresenham"
                             : "DDA"))
              << '\n';
    std::cout << simpleDuration.count() << ' ' << improvedDuration.count()
              << ' ' << ddaDuration.count() << std::endl;
}

TEST_CASE("Find bounding box of n-spaces object")
{
    std::vector model{
        std::array{0, 1, 3}, {-1, 2, 4}, {-5, 8, 10}, {120, 0, 3}};
    using iterType = std::vector<std::array<int, 3>>::const_iterator;
    auto actual = boundingBox<3, iterType>(model.cbegin(), model.cend());
    auto expected = std::array{-5, 120, 0, 8, 3, 10};
    CHECK_EQ(expected, actual);
}

TEST_CASE("Polygon triangulation")
{
    std::array triangle{std::pair{0, 0}, std::pair{10, 10}, std::pair{20, 0}};
    SUBCASE("Triangle")
    {
        std::vector<decltype(triangle)> triangles;
        polygonTriangulation(triangle.begin(), triangle.end(),
                             std::back_inserter(triangles));
        std::vector<decltype(triangle)> expected{
            {std::pair{0, 0}, {10, 10}, {20, 0}}};
        CHECK(triangles.size() == 1);
        CHECK(expected == triangles);
    }
    SUBCASE("Quad")
    {
        std::array quad{std::pair{0, 0}, std::pair{10, 10}, std::pair{15, 8},
                        std::pair{20, 0}};
        std::vector<decltype(triangle)> triangles;
        std::vector<decltype(triangle)> expected{
            {std::pair{0, 0}, {10, 10}, {15, 8}},
            {std::pair{0, 0}, {15, 8}, {20, 0}}};
        polygonTriangulation(quad.begin(), quad.end(),
                             std::back_inserter(triangles));
        CHECK(triangles.size() == 2);
        CHECK(expected == triangles);
    }
    SUBCASE("Pentagon")
    {
        std::array pentagon{std::pair{0, 0}, std::pair{10, 10},
                            std::pair{15, 8}, std::pair{18, 5},
                            std::pair{20, 0}};
        std::vector<decltype(triangle)> triangles;
        std::vector<decltype(triangle)> expected{
            {std::pair{0, 0}, {10, 10}, {15, 8}},
            {std::pair{0, 0}, {15, 8}, {18, 5}},
            {std::pair{0, 0}, {18, 5}, {20, 0}}};
        polygonTriangulation(pentagon.begin(), pentagon.end(),
                             std::back_inserter(triangles));
        CHECK(triangles.size() == 3);
        CHECK(expected == triangles);
    }
}
