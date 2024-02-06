#pragma once
#include "../../eng.h"

namespace eng::vec{

template <size_t dimensions>
class DimensionalVector : public std::array<floating, dimensions>{
public:
    DimensionalVector<dimensions>& normalize()
    {
        floating len {};
        for(auto value : *this){
            len += value * value;
        }
        len = std::sqrt(len);
        for(auto& value : *this){
            value /= len;
        }
        return *this;
    }

    DimensionalVector<dimensions>&
    operator-=(const DimensionalVector<dimensions>& b){
        auto& a = *this;
        for(unsigned i = 0; i < dimensions; i++)
            a[i] = a[i] - b[i];
        return a;
    }

    DimensionalVector<dimensions>&
    operator+=(const DimensionalVector<dimensions>& b){
        auto& a = *this;
        for(unsigned i = 0; i < dimensions; i++)
            a[i] = a[i] + b[i];
        return a;
    }

    DimensionalVector<dimensions>&
    operator/=(floating value){
        auto& a = *this;
        for(unsigned i = 0; i < dimensions; i++)
            a[i] /= value;
        return a;
    }


};

template<size_t dimensions> floating
operator*(const DimensionalVector<dimensions>& a, const DimensionalVector<dimensions>& b){
    floating result{};
    for(unsigned i = 0; i < dimensions; i++)
        result += a[i] * b[i];
    return result;
}

template<size_t dimensions> DimensionalVector<dimensions>
operator-(const DimensionalVector<dimensions>& a, const DimensionalVector<dimensions>& b){
    auto z = a;
    return z -= b;
}

template<size_t dimensions> DimensionalVector<dimensions>
operator+(const DimensionalVector<dimensions>& a, const DimensionalVector<dimensions>& b){
    auto z = a;
    return z *= b;
}

template<size_t dimensions> DimensionalVector<dimensions>
normalize(DimensionalVector<dimensions> from)
{
    return from.normalize();
}

using ThreeDimensionalVector = DimensionalVector<3>;
using FourDimensionalVector = DimensionalVector<4>;

ThreeDimensionalVector
vectorMultiplication(const ThreeDimensionalVector& a, const ThreeDimensionalVector& b) noexcept;

FourDimensionalVector threeDimensionsToFour(const ThreeDimensionalVector& a, floating w) noexcept;

}