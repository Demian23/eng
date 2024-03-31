#pragma once
#include "../../base/src/eng.h"
#include "Matrix.h"
namespace eng::mtr {

class Scale final : public Matrix {
public:
    explicit Scale(Vec3F scaleVector)
    {
        for (unsigned i = 0; i < scaleVector.size(); i++) {
            matrix_[i][i] = scaleVector[i];
        }
    }
};

} // namespace eng::mtr