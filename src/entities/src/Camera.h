#pragma once

#include "../../vector/src/DimensionalVector.h"
#include "../../matrix/src/View.h"

namespace eng::ent {

class Camera {
public:
    Camera(vec::ThreeDimensionalVector cameraEye,
           vec::ThreeDimensionalVector cameraTarget,
           vec::ThreeDimensionalVector cameraUp);

    mtr::Matrix getViewMatrix()const noexcept;

    void rotateX(double degree);
    void rotateY(double degree);
    void rotateZ(double degree);

    void moveAlongDiagonal(floating movement);
    void changePolarAngle(floating addition);
    void changeAzimuthalAngle(floating addition);

private:
    vec::FourDimensionalVector eye;
    vec::FourDimensionalVector target;
    vec::FourDimensionalVector up;

    void changeEyeAsSphericalVector(int index, floating addition);
};

} // namespace eng::ent

