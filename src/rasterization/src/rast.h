#pragma once

#include "../../algorithm/src/alg.h"
#include "../../vector/src/DimensionalVector.h"

namespace eng::rast {

using Triangle = std::array<eng::vec::Vec2I, 3>;
using namespace eng::alg;
using namespace eng::vec;

// TODO: shouldn't be inline -> move it to cpp
[[nodiscard]] inline std::array<eng::floating, 3>
barycentricCoordinates(eng::vec::Vec2I p, Triangle triangle)
{
    auto b_a = triangle[1] - triangle[0];
    auto c_a = triangle[2] - triangle[0];
    auto triangleSquare2x =
        static_cast<eng::floating>(std::abs(perpDot(b_a, c_a)));
    auto p_a = p - triangle[0];
    auto wB = static_cast<eng::floating>(std::abs(perpDot(p_a, c_a)));
    auto wC = static_cast<eng::floating>(std::abs(perpDot(p_a, b_a)));
    return {(triangleSquare2x - wB - wC) / triangleSquare2x,
            wB / triangleSquare2x, wC / triangleSquare2x};
}

template <typename Callback>
void generateBarycentricCoordinatesForEachPointInTriangle(eng::vec::Vec2F a,
                                                          eng::vec::Vec2F b,
                                                          eng::vec::Vec2F c,
                                                          Callback out)
{
    auto b_a = b - a;
    auto c_a = c - a;
    auto triangleSquare2x =
        static_cast<eng::floating>(std::abs(perpDot(b_a, c_a)));
    auto denominator = 1 / triangleSquare2x;
    auto xMin = static_cast<uint32_t>(std::floor(std::min({a[0], b[0], c[0]})));
    auto yMin = static_cast<uint32_t>(std::floor(std::min({a[1], b[1], c[1]})));
    auto xMax = static_cast<uint32_t>(std::floor(std::max({a[0], b[0], c[0]})));
    auto yMax = static_cast<uint32_t>(std::floor(std::max({a[1], b[1], c[1]})));
    for (auto y = yMin; y <= yMax; y++) {
        for (auto x = xMin; x <= xMax; x++) {
            auto p_a = eng::vec::Vec2F{static_cast<eng::floating>(x),
                                       static_cast<eng::floating>(y)} -
                       a;
            auto v = std::abs(perpDot(p_a, c_a)) * denominator;
            auto w = std::abs(perpDot(p_a, b_a)) * denominator;
            if (v >= 0 && w >= 0 && v + w <= static_cast<decltype(v)>(1)) {
                out(x, y, 1 - w - v, v, w);
            }
        }
    }
}

template <typename OutIter> void trianglePoints(Triangle triangle, OutIter out)
{
    using triangleIter = decltype(triangle.cbegin());

    auto [xMin, xMax, yMin, yMax] =
        boundingBox<triangleIter, 2>(triangle.cbegin(), triangle.cend());
    for (auto y = yMin; y <= yMax; y++) {
        for (auto x = xMin; x <= xMax; x++) {
            auto [u, v, w] = barycentricCoordinates({x, y}, triangle);
            ++u;
            if (v >= 0 && w >= 0 && v + w <= static_cast<decltype(w)>(1)) {
                *out = {x, y};
                ++out;
            }
        }
    }
}

} // namespace eng::rast