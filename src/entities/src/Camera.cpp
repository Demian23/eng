#include "Camera.h"
#include "../../matrix/src/View.h"
#include "../../matrix/src/Rotate.h"

namespace eng::ent {

Camera::Camera(vec::ThreeDimensionalVector cameraEye,
               vec::ThreeDimensionalVector cameraTarget,
               vec::ThreeDimensionalVector cameraUp) noexcept
    : eye{vec::cartesianToHomogeneous(cameraEye, 0)},
      target{vec::cartesianToHomogeneous(cameraTarget, 0)},
      up{vec::cartesianToHomogeneous(cameraUp, 0)}
{}

mtr::Matrix Camera::getViewMatrix() const noexcept
{
    return mtr::View{eye, target, up};
}

void Camera::rotateX(floating degree) noexcept
{
    eye = mtr::RotateX{degree} * eye;
}

void Camera::rotateY(floating degree) noexcept
{
    eye = mtr::RotateY{degree} * eye;
}

void Camera::rotateZ(floating degree) noexcept
{
    eye = mtr::RotateZ{degree} * eye;
}

void Camera::changeEyeAsSphericalVector(unsigned index,
                                        floating addition) noexcept
{
    auto sphericalRepresentation =
        vec::cartesianToSpherical(vec::homogeneousToCartesian(eye));
    sphericalRepresentation[index] += addition;
    eye = vec::cartesianToHomogeneous(
        vec::sphericalToCartesian(sphericalRepresentation), 0);
}

void Camera::moveAlongDiagonal(floating movement) noexcept
{
    changeEyeAsSphericalVector(0, movement);
}
void Camera::changePolarAngle(floating addition) noexcept
{
    changeEyeAsSphericalVector(1, addition);
}

void Camera::changeAzimuthalAngle(floating addition) noexcept
{
    changeEyeAsSphericalVector(2, addition);
}

} // namespace eng::ent
