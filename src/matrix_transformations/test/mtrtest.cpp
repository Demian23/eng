#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/Matrix.h"
#include "../src/MoveMatrix.h"
#include "../src/ScaleMatrix.h"
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

TEST_CASE("Vector multiplication with ScaleMatrix")
{
    FourDimensionalVector vector{3, 2, 1, 1}, expected{1.5, 4, 1, 1};
    ScaleMatrix scaleMatrix{{0.5, 2, 1}};
    auto result = scaleMatrix * vector;

    REQUIRE_EQ(expected, result);
}

TEST_CASE("Vector multiplication with MoveMatrix")
{
    FourDimensionalVector vector{3, 2, 1, 1}, expected{8, 7, -1, 1};
    MoveMatrix moveMatrix{{5, 5, -2}};
    auto result = moveMatrix * vector;

    REQUIRE_EQ(expected, result);
}
