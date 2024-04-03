#include "Camera.h"

namespace eng::ent {

Camera::Camera(vec::Vec3F cameraEye, vec::Vec3F cameraTarget,
               vec::Vec3F cameraUp) noexcept
    : eye{vec::cartesianToHomogeneous(vec::sphericalToCartesian(cameraEye), 0)},
      target{vec::cartesianToHomogeneous(cameraTarget, 1)},
      up{vec::cartesianToHomogeneous(cameraUp, 0)}
{}

void Camera::reset(vec::Vec3F cameraEye, vec::Vec3F cameraTarget,
                   vec::Vec3F cameraUp) noexcept
{
    eye = {
        vec::cartesianToHomogeneous(vec::sphericalToCartesian(cameraEye), 0)};
    target = {vec::cartesianToHomogeneous(cameraTarget, 1)};
    up = {vec::cartesianToHomogeneous(cameraUp, 0)};
}

mtr::Matrix Camera::getViewMatrix() const noexcept
{
    return mtr::Matrix::getView(eye, target, up);
}

void Camera::rotateX(floating degree) noexcept
{
    eye = mtr::Matrix::getRotateX(degree) * eye;
}

void Camera::rotateY(floating degree) noexcept
{
    eye = mtr::Matrix::getRotateY(degree) * eye;
}

void Camera::rotateZ(floating degree) noexcept
{
    eye = mtr::Matrix::getRotateZ(degree) * eye;
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
    changeEyeAsSphericalVector(1, degreeToRadian(addition));
}

void Camera::changeAzimuthalAngle(floating addition) noexcept
{
    changeEyeAsSphericalVector(2, degreeToRadian(addition));
}

void Camera::moveTarget(vec::Vec3F position) noexcept
{
    target = mtr::Matrix::getMove(position) * target;
}

[[nodiscard]] eng::floating Camera::getDiagonalLength() const noexcept
{
    return eye.length();
}

[[nodiscard]] vec::Vec3F Camera::getEye() const noexcept
{
    return vec::homogeneousToCartesian(eye);
}

[[nodiscard]] vec::Vec3F Camera::getTarget() const noexcept
{
    return vec::homogeneousToCartesian(target);
}

} // namespace eng::ent
