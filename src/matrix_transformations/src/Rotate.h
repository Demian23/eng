#pragma once

#include "Matrix.h"
#include <cmath>
#include <numbers>
#include <type_traits>

namespace eng::mtr {

template <typename T>
concept arithmetic = std::is_arithmetic_v<T>;
auto degree_to_rad(arithmetic auto degree) { return degree * M_PI / 180; }

class RotateX final : public Matrix {
public:
    template <typename T> RotateX(T degree)
    {
        matrix_[0][0] = 1.0f;
        matrix_[1][1] = matrix_[2][2] = std::cos(degree_to_rad(degree));
        matrix_[2][1] = std::sin(degree_to_rad(degree));
        matrix_[1][2] = -matrix_[2][1];
    }
};

class RotateY final : public Matrix {
public:
    template <typename T> RotateY(T degree)
    {
        matrix_[1][1] = 1.0f;
        matrix_[0][0] = matrix_[2][2] = std::cos(degree_to_rad(degree));
        matrix_[0][2] = std::sin(degree_to_rad(degree));
        matrix_[2][0] = -matrix_[0][2];
    }
};

class RotateZ final : public Matrix {
public:
    template <typename T> RotateZ(T degree)
    {
        matrix_[2][2] = 1.0f;
        matrix_[0][0] = matrix_[1][1] = std::cos(degree_to_rad(degree));
        matrix_[1][0] = std::sin(degree_to_rad(degree));
        matrix_[0][1] = -matrix_[1][0];
    }
};

} // namespace eng::mtr
