#pragma once

#include "Matrix.h"
#include <cmath>
#include <numbers>
#include <type_traits>

namespace eng::mtr {

class RotateX final : public Matrix {
public:
    explicit RotateX(floating degree)
    {
        matrix_[0][0] = 1.0f;
        matrix_[1][1] = matrix_[2][2] =
            std::cos(degreeToRadian(degree));
        matrix_[2][1] =
            std::sin(degreeToRadian(degree));
        matrix_[1][2] = -matrix_[2][1];
    }
};

class RotateY final : public Matrix {
public:
    explicit RotateY(floating degree)
    {
        matrix_[1][1] = 1.0f;
        matrix_[0][0] = matrix_[2][2] =
            std::cos(degreeToRadian(degree));
        matrix_[0][2] =
            std::sin(degreeToRadian(degree));
        matrix_[2][0] = -matrix_[0][2];
    }
};

class RotateZ final : public Matrix {
public:
    explicit RotateZ(floating degree)
    {
        matrix_[2][2] = 1.0f;
        matrix_[0][0] = matrix_[1][1] =
            std::cos(degreeToRadian(degree));
        matrix_[1][0] =
            std::sin(degreeToRadian(degree));
        matrix_[0][1] = -matrix_[1][0];
    }
};

} // namespace eng::mtr
