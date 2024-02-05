#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/Move.h"
#include "../src/Rotate.h"
#include "../src/Scale.h"
#include <doctest.h>

using namespace eng;
using namespace eng::mtr;

TEST_CASE("Matrix instantiation") { REQUIRE_NOTHROW(Matrix matrixInstance); }

TEST_CASE("Vector multiplication with identity matrix")
{
    FourDimensionalVector vector{3.0, 1.2, 4.5, 1};
    Matrix identityMatrix{{{{1.0f, 0.0f, 0.0f, 0.0f},
                            {0.0f, 1.0f, 0.0f, 0.0f},
                            {0.0f, 0.0f, 1.0f, 0.0f},
                            {0.0f, 0.0f, 0.0f, 1.0f}}}};

    auto result = identityMatrix * vector;
    REQUIRE_EQ(vector, result);
}

TEST_CASE("Matrix multiplication with matrix")
{
    Matrix a{{{
        {1, 2, 3, 4},
        {5, 6, 7, 8},
        {5, 2, 6, 7},
        {8, 1, 5, 7}
    }}};
    Matrix b{{{
        {2, 4, 4, 3},
        {1, 1, 2, 3},
        {4, 4, 8, 9},
        {9, 4, 6, 3}
    }}};

    Matrix expected{{{
        {52, 34, 56, 48},
        {116, 86, 136, 120},
        {99, 74, 114, 96},
        {100, 81, 116, 93}
    }}};

    auto result = a * b;

    REQUIRE_EQ(expected, result);
}

TEST_CASE("Vector multiplication with Scale")
{
    FourDimensionalVector vector{3, 2, 1, 1}, expected{1.5, 4, 1, 1};
    Scale scaleMatrix{{0.5, 2, 1}};
    auto result = scaleMatrix * vector;

    REQUIRE_EQ(expected, result);
}

TEST_CASE("Vector multiplication with Move")
{
    FourDimensionalVector vector{3, 2, 1, 1}, expected{8, 7, -1, 1};
    Move moveMatrix{{5, 5, -2}};
    auto result = moveMatrix * vector;

    REQUIRE_EQ(expected, result);
}

TEST_CASE("Vector rotate X 90")
{
    FourDimensionalVector vector{3, 2, 1, 1}, expected{3, -1, 2, 1};
    RotateX rotate{90};
    auto result = rotate * vector;
    CHECK_EQ(expected, result);
}

TEST_CASE("Vector rotate Y 90")
{
    FourDimensionalVector vector{3, 2, 1, 1}, expected{1, 2, -3, 1};
    RotateY rotate{90};
    auto result = rotate * vector;
    CHECK_EQ(expected, result);
}

TEST_CASE("Vector rotate Z 90")
{
    FourDimensionalVector vector{3, 2, 1, 1}, expected{-2, 3, 1, 1};
    RotateZ rotate{90};
    auto result = rotate * vector;
    CHECK_EQ(expected, result);
}
