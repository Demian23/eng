#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/DimensionalVector.h"
#include <doctest.h>

using namespace eng::vec;

TEST_CASE("Vector normalisation test")
{
    DimensionalVector<2> vector{3, 4};
    DimensionalVector<2> expected{3.0 / 5, 4.0 / 5};
    vector.normalize();
    REQUIRE_EQ(expected, vector);
}

TEST_CASE("Vector minus")
{
    DimensionalVector<2> a{3, 4}, b{8, -1.1};
    DimensionalVector<2> expected{-5, 5.1};
    a -= b;
    REQUIRE_EQ(expected, a);
}

TEST_CASE("Vector plus")
{
    DimensionalVector<2> a{3, 4}, b{8, -1.1};
    DimensionalVector<2> expected{11, 2.9};
    a += b;
    REQUIRE_EQ(expected, a);
}

TEST_CASE("Vector mul")
{
    DimensionalVector<2> a{3, 4}, b{8, 11};
    float expected{68};
    auto result = a * b;
    REQUIRE_EQ(expected, result);
}

TEST_CASE("Vector vector mul")
{
    ThreeDimensionalVector a{2, 3, 4}, b{5, 6, 7}, expected{-3, 6, -3};
    auto result = vectorMultiplication(a, b);
    REQUIRE_EQ(expected, result);
}
