#pragma once
#include "../../base/src/eng.h"
#include <algorithm>
#include <array>
#include <numeric>

namespace eng::vec {

template <size_t dimensions>
class DimensionalVector : public std::array<floating, dimensions> {
public:
    [[nodiscard]] floating length() const noexcept
    {
        return std::sqrt(std::accumulate(
            this->begin(), this->end(), static_cast<floating>(0),
            [](eng::floating res, eng::floating curr) {
                return res + curr * curr;
            }));
    }
    DimensionalVector<dimensions> &normalize()
    {
        floating len = length();
        std::transform(this->begin(), this->end(), this->begin(),
                       [len](auto &&value) { return value /= len; });
        return *this;
    }

    DimensionalVector<dimensions> &
    operator-=(const DimensionalVector<dimensions> &b)
    {
        auto &a = *this;
        for (unsigned i = 0; i < dimensions; i++)
            a[i] = a[i] - b[i];
        return a;
    }

    DimensionalVector<dimensions> &
    operator+=(const DimensionalVector<dimensions> &b)
    {
        auto &a = *this;
        for (unsigned i = 0; i < dimensions; i++)
            a[i] = a[i] + b[i];
        return a;
    }

    DimensionalVector<dimensions> &operator/=(floating value)
    {
        auto &a = *this;
        for (unsigned i = 0; i < dimensions; i++)
            a[i] /= value;
        return a;
    }
};

template <size_t dimensions>
floating operator*(const DimensionalVector<dimensions> &a,
                   const DimensionalVector<dimensions> &b)
{
    return std::inner_product(a.begin(), a.end(), b.begin(),
                              static_cast<floating>(0));
}

template <size_t dimensions>
DimensionalVector<dimensions> operator-(const DimensionalVector<dimensions> &a,
                                        const DimensionalVector<dimensions> &b)
{
    auto z = a;
    return z -= b;
}

template <size_t dimensions>
DimensionalVector<dimensions> operator+(const DimensionalVector<dimensions> &a,
                                        const DimensionalVector<dimensions> &b)
{
    auto z = a;
    return z *= b;
}

template <size_t dimensions>
DimensionalVector<dimensions> normalize(DimensionalVector<dimensions> from)
{
    return from.normalize();
}

using ThreeDimensionalVector = DimensionalVector<3>;
using FourDimensionalVector = DimensionalVector<4>;

FourDimensionalVector
vectorMultiplicationForHomogeneous(const FourDimensionalVector &a,
                                   const FourDimensionalVector &b) noexcept;

FourDimensionalVector cartesianToHomogeneous(const ThreeDimensionalVector &a,
                                             floating w) noexcept;

ThreeDimensionalVector
homogeneousToCartesian(const FourDimensionalVector &a) noexcept;

ThreeDimensionalVector sphericalToCartesian(
    const ThreeDimensionalVector &vectorInSphericalNotation) noexcept;

ThreeDimensionalVector
cartesianToSpherical(const ThreeDimensionalVector &cartesianVector) noexcept;

} // namespace eng::vec
