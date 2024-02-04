#pragma once
#include "../../eng.h"
#include "Matrix.h"
namespace eng::mtr {

class ScaleMatrix : public Matrix {
public:
    ScaleMatrix(ThreeDimensionalVector scaleVector)
    {
        for (unsigned i = 0; i < MatrixDimension - 1; i++) {
            matrix_[i][i] = scaleVector[i];
        }
    }
};

} // namespace eng::mtr