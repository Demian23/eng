#pragma once

#include "Matrix.h"

namespace eng::mtr {

class PerspectiveProjection final : public Matrix {
public:
    PerspectiveProjection(floating xMin, floating xMax, floating yMin,
                          floating yMax, floating zMin, floating zMax)
    {
        auto width = xMax - xMin;
        auto height = yMax - yMin;
        auto zDiff = zMin - zMax;
        matrix_[0][0] = (2 * zMin) / width;
        matrix_[1][1] = (2 * zMin) / height;
        matrix_[2][2] = zMax / zDiff;
        matrix_[2][3] = zMin * zMax / zDiff;
        matrix_[3][2] = -1;
        matrix_[3][3] = 0;
    }
};

} // namespace eng::mtr