#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "../src/DimensionalVector.h"
#include <doctest/doctest.h>

using namespace eng::vec;

TEST_CASE("Vector normalisation test")
{
    Vec2F vector{3, 4};
    Vec2F expected{3.0 / 5, 4.0 / 5};
    vector.normalize();
    CHECK_EQ(expected, vector);
}

TEST_CASE("Vector minus")
{
    Vec2F a{3, 4}, b{8, -1.1};
    Vec2F expected{-5, 5.1};
    a -= b;
    CHECK_EQ(expected, a);
}

TEST_CASE("Vector plus")
{
    Vec2F a{3, 4}, b{8, -1.1};
    Vec2F expected{11, 2.9};
    a += b;
    CHECK_EQ(expected, a);
}

TEST_CASE("Vector mul")
{
    Vec2F a{3, 4}, b{8, 11};
    float expected{68};
    auto result = a * b;
    CHECK_EQ(expected, result);
}

TEST_CASE("Vector vector mul")
{
    Vec4F a{2, 3, 4, 0}, b{5, 6, 7, 0}, expected{-3, 6, -3, 0};
    auto result = vectorMultiplicationForHomogeneous(a, b);
    CHECK_EQ(expected, result);
}

TEST_CASE("Vector perp")
{
    Vec2F a{10, 15};
    Vec2F expected{-15, 10};
    CHECK_EQ(expected, perp(a));
}

TEST_CASE("Vectors perp dot product")
{
    Vec2F a{0, 10}, b{5, 0};
    eng::floating expected = 50;
    CHECK_EQ(expected, std::abs(perpDot(a, b)));
}

TEST_CASE("Perp dot product as line side determinant")
{
    Vec2F a{-1, 1}, b{1, 0}, c{-5, -0.25};

    CHECK(perpDot(b, a) > 0);
    CHECK(perpDot(c, a) < 0);
}
