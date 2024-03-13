#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/alg.h"
#include <algorithm>
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
    line(0, 4, 3, 0, std::back_inserter(points));
    CHECK_EQ(expected, points);
}

TEST_CASE("ImprovedLine points generating")
{
    std::vector<valueType> points;
    std::vector<valueType> expected{{0, 3}, {1, 2}, {2, 2}, {3, 1}, {4, 0}};
    improvedBresenhamLine(0, 4, 3, 0, std::back_inserter(points));
    CHECK_EQ(expected, points);
}
TEST_CASE("DDALine points generating")
{
    std::vector<valueType> points;
    std::vector<valueType> expected{{0, 3}, {1, 2}, {2, 2}, {3, 1}, {4, 0}};
    ddaLine(0.0f, 4.0f, 3.0f, 0.0f, std::back_inserter(points));
    CHECK_EQ(expected, points);
}

TEST_CASE("ImprovedLine in difficult cases")
{
    std::vector<valueType> actual;
    std::vector<valueType> expected;

    line(4, 3, 5, 12, std::back_inserter(expected));
    improvedBresenhamLine(4, 3, 5, 12, std::back_inserter(actual));
    CHECK_EQ(expected, actual);

    line(4, 3, 3, 2, std::back_inserter(expected));
    improvedBresenhamLine(4, 3, 3, 2, std::back_inserter(actual));
    CHECK_EQ(expected, actual);

    line(0, 9, 3, 3, std::back_inserter(expected));
    improvedBresenhamLine(0, 9, 3, 3, std::back_inserter(actual));
    CHECK_EQ(expected, actual);
}

TEST_CASE("Line points output in ostream")
{
    std::string expected = "0,3 1,2 2,2 3,1 4,0 ";
    std::stringstream res;
    line(0, 4, 3, 0, std::ostream_iterator<valueType>(res, " "));
    REQUIRE_EQ(expected, res.str());
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

    constexpr int sampleSize = 10000;

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
    std::for_each(
        simpleLine.begin(), simpleLine.end(),
        [out = std::back_inserter(simpleLineResult)](auto lineRep) mutable {
            line(lineRep.x0y0.first, lineRep.x1y1.first, lineRep.x0y0.second,
                 lineRep.x1y1.second, out);
        });
    auto endSimple = std::chrono::high_resolution_clock::now();
    // sleep, cache?
    auto startImproved = std::chrono::high_resolution_clock::now();
    std::for_each(
        improvedLine.begin(), improvedLine.end(),
        [out = std::back_inserter(improvedLineResult)](auto lineRep) mutable {
            improvedBresenhamLine(lineRep.x0y0.first, lineRep.x1y1.first,
                                  lineRep.x0y0.second, lineRep.x1y1.second,
                                  out);
        });
    auto endImproved = std::chrono::high_resolution_clock::now();

    auto startDDA = std::chrono::high_resolution_clock::now();
    std::for_each(
        ddaLineInput.begin(), ddaLineInput.end(),
        [out = std::back_inserter(ddaLineResult)](auto lineRep) mutable {
            ddaLine(lineRep.x0y0.first, lineRep.x1y1.first, lineRep.x0y0.second,
                    lineRep.x1y1.second, out);
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
