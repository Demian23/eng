#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/Matrix.h"
#include <doctest/doctest.h>

using namespace eng;
using namespace eng::mtr;

TEST_CASE("Matrix instantiation") { REQUIRE_NOTHROW(Matrix matrixInstance); }

TEST_CASE("Vector multiplication with identity matrix")
{
    Vec4F vector{3.0f, 1.2f, 4.5f, 1.0f};
    auto result = Matrix::createIdentityMatrix() * vector;
    REQUIRE_EQ(vector, result);
}

TEST_CASE("Matrix multiplication with matrix")
{
    Matrix a{{{{1, 2, 3, 4}, {5, 6, 7, 8}, {5, 2, 6, 7}, {8, 1, 5, 7}}}};
    Matrix b{{{{2, 4, 4, 3}, {1, 1, 2, 3}, {4, 4, 8, 9}, {9, 4, 6, 3}}}};

    Matrix expected{{{{52, 34, 56, 48},
                      {116, 86, 136, 120},
                      {99, 74, 114, 96},
                      {100, 81, 116, 93}}}};

    auto result = a * b;

    REQUIRE_EQ(expected, result);
}

TEST_CASE("Vector multiplication with Scale")
{
    Vec4F vector{3, 2, 1, 1}, expected{1.5, 4, 1, 1};
    auto scaleMatrix = Matrix::getScale({0.5, 2, 1});
    auto result = scaleMatrix * vector;

    REQUIRE_EQ(expected, result);
}

TEST_CASE("Vector multiplication with Move")
{
    Vec4F vector{3, 2, 1, 1}, expected{8, 7, -1, 1};
    auto moveMatrix = Matrix::getMove({5, 5, -2});
    auto result = moveMatrix * vector;

    REQUIRE_EQ(expected, result);
}
