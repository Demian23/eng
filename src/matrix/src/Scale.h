#pragma once
#include "../../eng.h"
#include "Matrix.h"
namespace eng::mtr {

class Scale final : public Matrix {
public:
    explicit Scale(ThreeDimensionalVector scaleVector)
    {
        for (unsigned i = 0; i < scaleVector.size(); i++) {
            matrix_[i][i] = scaleVector[i];
        }
    }
};

} // namespace eng::mtr