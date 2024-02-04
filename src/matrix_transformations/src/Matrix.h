#pragma once
#include "../../eng.h"
#include <array>

namespace eng::mtr {

enum { MatrixDimension = 4 };

using MatrixRepresentation =
    std::array<std::array<floating, MatrixDimension>, MatrixDimension>;

class Matrix {
public:
    Matrix() : matrix_{}
    {
        matrix_[MatrixDimension - 1][MatrixDimension - 1] = 1.0f;
    }

    Matrix(MatrixRepresentation sourceMatrix) : matrix_(sourceMatrix) {}

    [[nodiscard]] FourDimensionalVector
    operator*(FourDimensionalVector vector) const noexcept;

    [[nodiscard]] Matrix
    operator*(const Matrix& matrix)const noexcept;

protected:
    MatrixRepresentation matrix_;
};

} // namespace eng::mtr
