#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/rast.h"
#include "doctest/doctest.h"
#include <utility>

using namespace eng::rast;

TEST_CASE("Barycentric coordinates")
{
    Triangle triangle{{{0, 0}, {10, 10}, {20, 0}}};
    SUBCASE("in triangle vertex")
    {
        Vec2I p{0, 0};
        auto actual = barycentricCoordinates(p, triangle);
        decltype(actual) expected = {1, 0, 0};
        CHECK_EQ(expected, actual);
    }
    SUBCASE("on triangle edge")
    {
        Vec2I p{15, 0};
        auto actual = barycentricCoordinates(p, triangle);
        decltype(actual) expected = {0.25, 0, 0.75};
        CHECK_EQ(expected, actual);
    }
    SUBCASE("in triangle center, but closer to B")
    {
        Vec2I p{10, 7};
        auto actual = barycentricCoordinates(p, triangle);
        decltype(actual) expected{0.15, 0.7, 0.15};
        REQUIRE_EQ(expected, actual);
    }
}

TEST_CASE("Points generating")
{
    Triangle triangle{{{2, 1}, {4, 4}, {5, 1}}};
    std::set<std::pair<eng::floating, eng::floating>> actual{};
    std::set<std::pair<eng::floating, eng::floating>> expected{{{2, 1}, {3, 1}}};

    trianglePoints(triangle, std::inserter(actual, actual.begin()));
    CHECK_NE(actual, expected);
}
