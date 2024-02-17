#pragma once

#include <numbers>

namespace eng {

using numeric = long;
using floating = float;

inline floating degreeToRadian(floating degree)
{
    return degree * std::numbers::pi_v<floating> / 180;
}

} // namespace eng
