#include "CameraProjection.h"

namespace eng::ent {

CameraProjection::CameraProjection(floating w, floating h, floating zMin,
                                   floating zMax,
                                   floating projectionAngle) noexcept
    : width{w}, height{h}, zNear{zMin}, zFar{zMax}, angle{projectionAngle}
{}

mtr::Matrix CameraProjection::getProjectionMatrix(
    ProjectionType projectionType) const noexcept
{
    switch (projectionType) {
    case ProjectionType::Orthographic:
        return mtr::Matrix::getOrthographicProjection(width, height, zNear,
                                                      zFar);
    case ProjectionType::Perspective:
        return mtr::Matrix::getPerspectiveProjectionWithAngle(
            degreeToRadian(angle), width / height, zNear, zFar);
    }
    return mtr::Matrix::createIdentityMatrix();
}

void CameraProjection::reset(floating w, floating h, floating zMin,
                             floating zMax, floating projectionAngle) noexcept
{
    width = w;
    height = h;
    zNear = zMin;
    zFar = zMax;
    angle = projectionAngle;
}

void CameraProjection::setZComponent(eng::floating zMin,
                                     eng::floating zMax) noexcept
{
    zNear = zMin;
    zFar = zMax;
}

[[nodiscard]] floating CameraProjection::getAngleInDegrees() const noexcept
{
    return angle;
}

void CameraProjection::setAngleInDegrees(floating newAngle) noexcept
{
    angle = newAngle;
}

[[nodiscard]] floating CameraProjection::getAspect() const noexcept
{
    return width / height;
}

[[nodiscard]] floating CameraProjection::getZMin() const noexcept
{
    return zNear;
}

[[nodiscard]] std::pair<eng::floating, eng::floating>
CameraProjection::getZComponent() const noexcept
{
    return {zNear, zFar};
}

} // namespace eng::ent
