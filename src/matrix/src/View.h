#pragma once

#include "../../vector/src/DimensionalVector.h"
#include "Matrix.h"
namespace eng::mtr {

class View final : public Matrix {
public:
    // w skip cause?
    View(Vec4F eye, Vec4F target, Vec4F up)
    {
        auto zAxis = (eye - target).normalize();
        auto xAxis =
            (vec::vectorMultiplicationForHomogeneous(zAxis, up)).normalize();
        auto yAxis = up; // vec::vectorMultiplication(xAxis, zAxis); // ?

        xAxis[3] = -(xAxis * eye);
        yAxis[3] = -(yAxis * eye);
        zAxis[3] = -(zAxis * eye);

        matrix_[0] = xAxis;
        matrix_[1] = yAxis;
        matrix_[2] = zAxis;
    }
};

} // namespace eng::mtr