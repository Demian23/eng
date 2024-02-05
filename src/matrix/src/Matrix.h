#pragma once
#include "../../eng.h"
#include "../../vector/src/DimensionalVector.h"
#include <array>

namespace eng::mtr {

using eng::vec::ThreeDimensionalVector;
using eng::vec::FourDimensionalVector;

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

    // TODO implement matrix multiplication
    [[nodiscard]] Matrix operator*(const Matrix &matrix) const noexcept;

    inline bool operator==(const Matrix& matrix)const noexcept{return matrix_ == matrix.matrix_;}
    inline bool operator!=(const Matrix& matrix)const noexcept{return matrix_ != matrix.matrix_;}

protected:
    MatrixRepresentation matrix_;
};

} // namespace eng::mtr
