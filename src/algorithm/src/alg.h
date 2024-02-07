#pragma once

#include "../../eng.h"

namespace eng::alg {

// Bresenham's line
template <typename valueType, typename OutIterator>
void line(numeric x0, numeric x1, numeric y0, numeric y1, OutIterator&& out)
{
    auto deltaX = std::abs(x1 - x0);
    auto deltaY = std::abs(y1 - y0);
    numeric error{};
    auto deltaError = deltaY + 1;
    auto y = y0;
    auto yDirection = y1 - y0 > 0 ? 1 : -1;
    for (auto x = x0; x <= x1; ++x, ++out) {
        *out = valueType {x, y};
        error += deltaError;
        if (error >= deltaX + 1) {
            y += yDirection;
            error -= deltaX + 1;
        }
    }
}

} // namespace eng::alg