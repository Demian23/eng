#pragma once
#include "Matrix.h"

namespace eng::mtr{

class Viewport : public Matrix{
public:
    Viewport(numeric xMin, numeric xMax, numeric yMin, numeric yMax){
        auto width = (xMax - xMin);
        auto height = (yMax - yMin);
        matrix_[0][0] = width/2;
        matrix_[1][1] = -height/2;
        matrix_[2][2] = 1;
        matrix_[0][3] = xMin + width/2;
        matrix_[1][3] = yMin + height/2;
    }
};

}