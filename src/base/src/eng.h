#pragma once

#include <numbers>

namespace eng {

using integral = long long;
using floating = float;

inline floating degreeToRadian(floating degree)
{
    return degree * std::numbers::pi_v<floating> / 180;
}
inline floating radianToDegree(floating radian)
{
    return radian * 180 / std::numbers::pi_v<floating>;
}

} // namespace eng
