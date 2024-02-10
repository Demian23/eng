#pragma once
#include "Matrix.h"
namespace eng::mtr {

class Move final : public Matrix {
public:
    explicit Move(ThreeDimensionalVector moveVector)
    {
        for (unsigned i = 0; i < MatrixDimension - 1; i++) {
            matrix_[i][i] = 1.0f;
        }
        for (unsigned i = 0; i < MatrixDimension - 1; i++) {
            matrix_[i][3] = moveVector[i];
        }
    }
};

} // namespace eng::mtr
