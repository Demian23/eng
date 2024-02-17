#pragma once

#include "../../base/src/eng.h"
#include <cmath>

namespace eng::alg {

// Bresenham's line
template <typename OutIterator>
void line(numeric x0, numeric x1, numeric y0, numeric y1, OutIterator &&out)
{
    auto deltaX = std::abs(x1 - x0);
    auto deltaY = std::abs(y1 - y0);
    auto signX = (x0 < x1) ? 1 : -1;
    auto signY = (y0 < y1) ? 1 : -1;
    auto error = deltaX - deltaY;
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

} // namespace eng::alg