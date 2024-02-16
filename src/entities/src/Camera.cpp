#include "Camera.h"
#include "../../matrix/src/Rotate.h"

namespace eng::ent{

Camera::Camera(vec::ThreeDimensionalVector cameraEye,
vec::ThreeDimensionalVector cameraTarget,
    vec::ThreeDimensionalVector cameraUp)
    : eye{vec::cartesianToHomogeneous(cameraEye, 0)},
    target{vec::cartesianToHomogeneous(cameraTarget, 0)},
    up{vec::cartesianToHomogeneous(cameraUp, 0)}
{}

mtr::Matrix Camera::getViewMatrix()const noexcept
{
    return mtr::View{eye, target, up};
}

void Camera::rotateX(double degree)
{
    eye = mtr::RotateX{degree} * eye;
}

void Camera::rotateY(double degree)
{
    eye = mtr::RotateY{degree} * eye;
}

void Camera::rotateZ(double degree){
    eye = mtr::RotateZ{degree} * eye;
}

void Camera::changeEyeAsSphericalVector(int index, floating addition)
{
    auto sphericalRepresentation = vec::cartesianToSpherical(vec::homogeneousToCartesian(eye));
    sphericalRepresentation[index] += addition;
    eye = vec::cartesianToHomogeneous(vec::sphericalToCartesian(sphericalRepresentation), 0);
}

void Camera::moveAlongDiagonal(floating movement)
{
    changeEyeAsSphericalVector(0, movement);
}
void Camera::changePolarAngle(floating addition)
{
    changeEyeAsSphericalVector(1, addition);
}

void Camera::changeAzimuthalAngle(floating addition)
{
    changeEyeAsSphericalVector(2, addition);
}

}