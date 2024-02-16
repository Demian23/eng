#pragma once

#include "Matrix.h"
#include <cmath>
#include <numbers>
#include <type_traits>

namespace eng::mtr {

class RotateX final : public Matrix {
public:
    explicit RotateX(double degree)
    {
        matrix_[0][0] = 1.0f;
        matrix_[1][1] = matrix_[2][2] =
            static_cast<eng::floating>(std::cos(degreeToRadian(degree)));
        matrix_[2][1] =
            static_cast<eng::floating>(std::sin(degreeToRadian(degree)));
        matrix_[1][2] = -matrix_[2][1];
    }
};

class RotateY final : public Matrix {
public:
    explicit RotateY(double degree)
    {
        matrix_[1][1] = 1.0f;
        matrix_[0][0] = matrix_[2][2] =
            static_cast<eng::floating>(std::cos(degreeToRadian(degree)));
        matrix_[0][2] =
            static_cast<eng::floating>(std::sin(degreeToRadian(degree)));
        matrix_[2][0] = -matrix_[0][2];
    }
};

class RotateZ final : public Matrix {
public:
    explicit RotateZ(double degree)
    {
        matrix_[2][2] = 1.0f;
        matrix_[0][0] = matrix_[1][1] =
            static_cast<eng::floating>(std::cos(degreeToRadian(degree)));
        matrix_[1][0] =
            static_cast<eng::floating>(std::sin(degreeToRadian(degree)));
        matrix_[0][1] = -matrix_[1][0];
    }
};

} // namespace eng::mtr
