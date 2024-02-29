#pragma once

#include "../../base/src/eng.h"
#include <algorithm>
#include <cmath>

namespace eng::alg {

// Bresenham's line
template <typename OutIterator>
void line(numeric x0, numeric x1, numeric y0, numeric y1, OutIterator &&out)
{
    int64_t deltaX = std::abs(x1 - x0);
    int64_t deltaY = std::abs(y1 - y0);
    auto signX = (x0 < x1) ? 1 : -1;
    auto signY = (y0 < y1) ? 1 : -1;
    int64_t error = deltaX - deltaY;
    for (auto x = x0, y = y0;;) {
        *out = {x, y};
        ++out;
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

template <typename OutIterator>
void improvedBresenhamLine(numeric x0, numeric x1, numeric y0, numeric y1,
                           OutIterator &&out)
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

    for (numeric x = 0, y = 0; x <= incrementSideOfLineRectangle; x++) {
        auto xt = x0 + m11 * x + m12 * y;
        auto yt = y0 + m21 * x + m22 * y;
        *out = {xt, yt};
        ++out;
        error += errorInc;
        if (error > incrementSideOfLineRectangle) {
            error -= errorDec;
            y++;
        }
    }
}

template <typename OutIterator>
void ddaLine(floating x0, floating x1, floating y0, floating y1,
             OutIterator &&out)
{
    auto xStart = static_cast<numeric>(std::round(x0));
    auto xEnd = static_cast<numeric>(std::round(x1));
    auto yStart = static_cast<numeric>(std::round(y0));
    auto yEnd = static_cast<numeric>(std::round(y1));
    auto width = xEnd - xStart;
    auto height = yEnd - yStart;
    auto incrementSideOfLineRectangle =
        std::max(std::abs(width), std::abs(height));
    auto x = x0, y = y0;
    auto xStep = static_cast<floating>(width) /
                 static_cast<floating>(incrementSideOfLineRectangle);
    auto yStep = static_cast<floating>(height) /
                 static_cast<floating>(incrementSideOfLineRectangle);
    for (numeric i = 0; i <= incrementSideOfLineRectangle; i++) {
        // get this values from out iterator?
        *out = {static_cast<numeric>(std::round(x)),
                static_cast<numeric>(std::round(y))};
        // accuracy?
        x += xStep;
        y += yStep;
    }
}

} // namespace eng::alg