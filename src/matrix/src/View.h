#pragma once

#include "Matrix.h"
#include "../../vector/src/DimensionalVector.h"
namespace eng::mtr{

class View final: public Matrix{
public:
    // w skip cause?
    View(ThreeDimensionalVector eye, ThreeDimensionalVector target,
         ThreeDimensionalVector up)
    {
        auto zAxis = (eye - target).normalize();
        auto xAxis = (vec::vectorMultiplication(zAxis, up)).normalize();
        auto yAxis = up;
        matrix_[0] = vec::threeDimensionsToFour(xAxis, -(xAxis * eye));
        matrix_[1] = vec::threeDimensionsToFour(yAxis, -(yAxis * eye));
        matrix_[1] = vec::threeDimensionsToFour(zAxis, -(zAxis * eye));
    }
};

}