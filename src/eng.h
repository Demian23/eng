#pragma once

#include <array>

namespace eng {

using numeric = long;
using floating = float;

template <unsigned size> using DimensionalVector = std::array<floating, size>;

using ThreeDimensionalVector = DimensionalVector<3>;
using FourDimensionalVector = DimensionalVector<4>;

} // namespace eng
