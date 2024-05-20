#pragma once

#include "../../base/src/eng.h"
#include "../../vector/src/DimensionalVector.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <functional>
#include <utility>

namespace eng::alg {

// Bresenham's line
template <typename Out>
void line(integral x0, integral x1, integral y0, integral y1, Out &&out)
{
    int64_t deltaX = std::abs(x1 - x0);
    int64_t deltaY = std::abs(y1 - y0);
    auto signX = (x0 < x1) ? 1 : -1;
    auto signY = (y0 < y1) ? 1 : -1;
    int64_t error = deltaX - deltaY;
    for (auto x = x0, y = y0;;) {
        out(x, y);
        if (x == x1 && y == y1)
            break;
        auto errorForCheck = error * 2;
        if (errorForCheck > -deltaY) {
            error -= deltaY;
            x += signX;
        }
        if (errorForCheck < deltaX) {
            error += deltaX;
            y += signY;
        }
    }
}

template <typename Out>
void improvedBresenhamLine(integral x0, integral x1, integral y0, integral y1,
                           Out &&out)
{
    auto lineWidth = std::abs(x1 - x0);
    auto lineHeight = std::abs(y1 - y0);
    auto m11 = (x0 < x1) ? 1 : -1;
    auto m12 = 0;
    auto m21 = 0;
    auto m22 = (y0 < y1) ? 1 : -1;
    auto incrementSideOfLineRectangle = std::max(lineWidth, lineHeight);
    if (lineWidth < lineHeight) {
        m12 = std::exchange(m11, m12);
        m22 = std::exchange(m21, m22);
    }

    int64_t error = 0;
    int64_t errorDec = 2 * incrementSideOfLineRectangle;
    int64_t errorInc = 2 * std::min(lineWidth, lineHeight);

    for (integral x = 0, y = 0; x <= incrementSideOfLineRectangle; x++) {
        auto xt = x0 + m11 * x + m12 * y;
        auto yt = y0 + m21 * x + m22 * y;
        out(xt, yt);
        error += errorInc;
        if (error > incrementSideOfLineRectangle) {
            error -= errorDec;
            y++;
        }
    }
}

template <typename Out>
void ddaLine(floating x0, floating x1, floating y0, floating y1, Out &&out)
{
    auto xStart = static_cast<integral>(std::round(x0));
    auto xEnd = static_cast<integral>(std::round(x1));
    auto yStart = static_cast<integral>(std::round(y0));
    auto yEnd = static_cast<integral>(std::round(y1));
    auto width = xEnd - xStart;
    auto height = yEnd - yStart;
    auto incrementSideOfLineRectangle =
        std::max(std::abs(width), std::abs(height));
    auto x = x0, y = y0;
    auto xStep = static_cast<floating>(width) /
                 static_cast<floating>(incrementSideOfLineRectangle);
    auto yStep = static_cast<floating>(height) /
                 static_cast<floating>(incrementSideOfLineRectangle);
    for (integral i = 0; i <= incrementSideOfLineRectangle; i++) {
        // get this values from out iterator?
        out(static_cast<integral>(std::round(x)),
            static_cast<integral>(std::round(y)));
        // accuracy?
        x += xStep;
        y += yStep;
    }
}

template <size_t dimensions, typename Iter,
          typename Coord = std::iterator_traits<Iter>::value_type::value_type>
std::array<Coord, dimensions * 2> boundingBox(Iter begin, Iter end)
{
    std::array<Coord, dimensions * 2> result{};
    auto comparatorGenerator = [](unsigned addition) {
        return [addition](auto &&firstVertex, auto &&secondVertex) {
            return *(firstVertex.begin() + addition) <
                   *(secondVertex.begin() + addition);
        };
    };
    for (unsigned i = 0; i < dimensions; i++) {
        auto compareIComponent = comparatorGenerator(i);
        result[i * 2] = (*std::min_element(begin, end, compareIComponent))[i];
        result[i * 2 + 1] =
            (*std::max_element(begin, end, compareIComponent))[i];
    }
    return result;
}

template <typename VertexIter, typename Out>
void polygonTriangulation(VertexIter begin, VertexIter end, Out out)
{
    auto n = end - begin;
    for (auto i = 1; i < n - 1; i++, ++out)
        *out = {*begin, *(begin + i), *(begin + i + 1)};
}

template <typename Out>
void drawTriangle(vec::Vec2I a, vec::Vec2I b, vec::Vec2I c, Out inserter)
{
    eng::alg::line(a[0], c[0], a[1], c[1], inserter);
    eng::alg::line(a[0], b[0], a[1], b[1], inserter);
    eng::alg::line(b[0], c[0], b[1], c[1], inserter);
}

template <typename T>
concept ZBufferCheck = std::invocable<T, uint32_t, uint32_t, floating>;

template <typename T>
concept BCOutput =
    std::invocable<T, uint32_t, uint32_t, floating, floating, floating>;

void barycentricCoordinates(vec::Vec3F a, vec::Vec3F b, vec::Vec3F c,
                            ZBufferCheck auto zCheck, BCOutput auto out)
{
    using namespace eng::vec;
    using std::ceil, std::min, std::max;

    auto a_b = (b - a).trim<2>();
    auto a_c = (c - a).trim<2>();
    auto b2 = b.trim<2>();
    auto c2 = c.trim<2>();
    auto a2 = a.trim<2>();
    numeric auto triangleSquare2x = perpDot(a_c, a_b);
    numeric auto denominator = 1 / triangleSquare2x;

    // potentially bad assumption that there is no negative coordinates
    // TODO make it accurate or clamp and then cast
    auto xMin = static_cast<uint32_t>((ceil(min({a[0], b[0], c[0]}))));
    auto yMin = static_cast<uint32_t>(ceil(min({a[1], b[1], c[1]})));
    auto xMax = static_cast<uint32_t>(ceil(max({a[0], b[0], c[0]})));
    auto yMax = static_cast<uint32_t>(ceil(max({a[1], b[1], c[1]})));
    for (auto y = yMin; y <= yMax; y++) {
        for (auto x = xMin; x <= xMax; x++) {
            auto p = Vec2F{static_cast<floating>(x), static_cast<floating>(y)};
            auto p_c = c2 - p;
            auto p_b = b2 - p;
            auto p_a = a2 - p;

            numeric auto u = perpDot(p_c, p_b) * denominator;
            numeric auto v = perpDot(p_a, p_c) * denominator;
            numeric auto w = 1 - u - v;
            if (v >= 0 && w >= 0 && u >= 0) {
                // TODO potentially not accurate
                auto z = a[2] * u + b[2] * v + c[2] * w;
                if (std::invoke(zCheck, x, y, z))
                    std::invoke(out, x, y, u, v, w);
            }
        }
    }
}

} // namespace eng::alg
