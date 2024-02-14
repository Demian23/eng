#pragma once

#include "../../vector/src/DimensionalVector.h"
#include "Matrix.h"
namespace eng::mtr {

class View final : public Matrix {
public:
    // w skip cause?
    View(ThreeDimensionalVector eye, ThreeDimensionalVector target,
         ThreeDimensionalVector up = {0, 1, 0})
    {
        auto zAxis = (eye - target).normalize();
        auto xAxis = (vec::vectorMultiplication(zAxis, up)).normalize();
        auto yAxis = up;//vec::vectorMultiplication(xAxis, zAxis); // ?
        matrix_[0] = vec::cartesianToHomogeneous(xAxis, -(xAxis * eye));
        matrix_[1] = vec::cartesianToHomogeneous(yAxis, -(yAxis * eye));
        matrix_[2] = vec::cartesianToHomogeneous(zAxis, -(zAxis * eye));
    }
};

} // namespace eng::mtr