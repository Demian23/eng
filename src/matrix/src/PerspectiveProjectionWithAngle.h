#pragma once

#include "Matrix.h"

namespace eng::mtr {

class PerspectiveProjectionWithAngle : public Matrix {
public:
    PerspectiveProjectionWithAngle(floating degreeInRad, floating aspect,
                                   floating zMin, floating zMax)
    {
        auto zDiff = zMin - zMax;
        auto angle = static_cast<floating>(degreeInRad / 2);
        matrix_[0][0] = static_cast<floating>(1 / (aspect * std::tan(angle)));
        matrix_[1][1] = static_cast<floating>(1 / std::tan(angle));
        matrix_[2][2] = zMax / zDiff;
        matrix_[2][3] = zMin * zMax / zDiff;
        matrix_[3][2] = -1;
        matrix_[3][3] = 0;
    }
};

} // namespace eng::mtr