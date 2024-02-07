#pragma once

#include "Matrix.h"

namespace eng::mtr {

class OrthographicProjection : public Matrix {
public:
    OrthographicProjection(floating xMin, floating xMax, floating yMin,
                           floating yMax, floating zMin, floating zMax)
    {
        auto width = xMax - xMin;
        auto height = yMax - yMin;
        matrix_[0][0] = 2.0f / width;
        matrix_[1][1] = 2.0f / height;
        matrix_[2][2] = 1.0f / (zMin - zMax);
        matrix_[2][3] = zMin / (zMin - zMax);
    }
};

} // namespace eng::mtr