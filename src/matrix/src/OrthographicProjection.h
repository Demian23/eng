#pragma once

#include "Matrix.h"

namespace eng::mtr{

class OrthographicProjection : public Matrix{
public:
    OrthographicProjection(numeric xMin, numeric xMax, numeric yMin, numeric yMax, numeric zMin, numeric zMax){
        floating width = xMax - xMin;
        floating height = yMax - yMin;
        matrix_[0][0] = 2.0f/(xMax - xMin);
        matrix_[1][1] = 2.0f/height;
        matrix_[2][2] = 1.0f/(zMin - zMax);
        matrix_[2][3] = zMin/(zMin - zMax);
    }
};

}