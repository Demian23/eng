#pragma once
#include "../../base/src/eng.h"
#include <algorithm>
#include <array>
#include <numeric>

namespace eng::vec {

template <typename T>
concept numeric = std::is_integral_v<T> || std::is_floating_point_v<T>;

template <size_t dimensions, numeric Component>
class DimensionalVector : public std::array<Component, dimensions> {
public:
    [[nodiscard]] floating length() const noexcept
    {
        return std::sqrt(std::accumulate(
            this->begin(), this->end(), static_cast<floating>(0),
            [](eng::floating res, eng::floating curr) {
                return res + curr * curr;
            }));
    }
    DimensionalVector<dimensions, Component> &normalize()
    {
        floating len = length();
        std::transform(this->begin(), this->end(), this->begin(),
                       [len](auto &&value) { return value /= len; });
        return *this;
    }

    DimensionalVector<dimensions, Component> &
    operator-=(const DimensionalVector<dimensions, Component> &b)
    {
        auto &a = *this;
        for (unsigned i = 0; i < dimensions; i++)
            a[i] = a[i] - b[i];
        return a;
    }

    DimensionalVector<dimensions, Component> &
    operator+=(const DimensionalVector<dimensions, Component> &b)
    {
        auto &a = *this;
        for (unsigned i = 0; i < dimensions; i++)
            a[i] = a[i] + b[i];
        return a;
    }

    DimensionalVector<dimensions, Component> &operator/=(floating value)
    {
        auto &a = *this;
        for (unsigned i = 0; i < dimensions; i++)
            a[i] /= value;
        return a;
    }

    template <size_t newSize>
    constexpr DimensionalVector<newSize, Component> trim() const noexcept
    {
        DimensionalVector<newSize, Component> result{};
        std::copy_n(this->begin(), newSize, result.begin());
        return result;
    }

    template <size_t newSize, typename NewComponent>
    constexpr DimensionalVector<newSize, NewComponent> convert() const noexcept
    {
        DimensionalVector<newSize, NewComponent> result{};
        std::transform(this->begin(), this->begin() + newSize, result.begin(),
                       [](auto &&component) {
                           return static_cast<NewComponent>(component);
                       });
        return result;
    }
};

template <size_t dimensions, numeric Component>
floating operator*(const DimensionalVector<dimensions, Component> &a,
                   const DimensionalVector<dimensions, Component> &b)
{
    return std::inner_product(a.begin(), a.end(), b.begin(),
                              static_cast<floating>(0));
}

template <size_t dimensions, numeric Component>
DimensionalVector<dimensions, Component>
operator-(const DimensionalVector<dimensions, Component> &a,
          const DimensionalVector<dimensions, Component> &b)
{
    auto z = a;
    return z -= b;
}

template <size_t dimensions, numeric Component>
DimensionalVector<dimensions, Component>
operator+(const DimensionalVector<dimensions, Component> &a,
          const DimensionalVector<dimensions, Component> &b)
{
    auto z = a;
    return z *= b;
}

template <size_t dimensions, numeric Component>
DimensionalVector<dimensions, Component>
normalize(DimensionalVector<dimensions, Component> from)
{
    return from.normalize();
}

template <size_t dimensions>
using IntegralVector = DimensionalVector<dimensions, integral>;

template <size_t dimensions>
using FloatingVector = DimensionalVector<dimensions, floating>;

using Vec2F = FloatingVector<2>;
using Vec3F = FloatingVector<3>;
using Vec4F = FloatingVector<4>;

using Vec2I = IntegralVector<2>;
template <numeric Component> using Vec2 = DimensionalVector<2, Component>;
template <numeric Component> using Vec3 = DimensionalVector<3, Component>;

template <numeric Component> Vec2<Component> perp(Vec2<Component> a) noexcept
{
    a[0] = -std::exchange(a[1], a[0]);
    return a;
}

template <numeric Component>
Component perpDot(const Vec2<Component> &a, const Vec2<Component> &b) noexcept
{
    return a[0] * b[1] - a[1] * b[0];
}

template <numeric Component>
Vec3<Component> cross(Vec3<Component> a, Vec3<Component> b) noexcept
{
    Vec3<Component> result;
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    return result;
}

Vec4F vectorMultiplicationForHomogeneous(const Vec4F &a,
                                         const Vec4F &b) noexcept;

Vec4F cartesianToHomogeneous(const Vec3F &a, floating w) noexcept;

Vec3F homogeneousToCartesian(const Vec4F &a) noexcept;

Vec3F sphericalToCartesian(const Vec3F &vectorInSphericalNotation) noexcept;

Vec3F cartesianToSpherical(const Vec3F &cartesianVector) noexcept;

} // namespace eng::vec
