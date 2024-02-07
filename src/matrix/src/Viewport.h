#pragma once
#include "Matrix.h"

namespace eng::mtr {

class Viewport : public Matrix {
public:
    Viewport(floating xMin, floating xMax, floating yMin, floating yMax)
    {
        auto width = (xMax - xMin);
        auto height = (yMax - yMin);
        matrix_[0][0] = width / 2;
        matrix_[1][1] = -height / 2;
        matrix_[2][2] = 1;
        matrix_[0][3] = xMin + width / 2;
        matrix_[1][3] = yMin + height / 2;
    }
};

} // namespace eng::mtr