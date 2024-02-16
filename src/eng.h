#pragma once

#include <numbers>

namespace eng {

using numeric = long;
using floating = float;

double degreeToRadian(std::floating_point auto degree)
{
    return degree * std::numbers::pi / 180;
}

} // namespace eng
