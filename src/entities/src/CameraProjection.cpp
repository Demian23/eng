#include "CameraProjection.h"
#include "../../matrix/src/OrthographicProjection.h"
#include "../../matrix/src/PerspectiveProjectionWithAngle.h"

namespace eng::ent {

mtr::Matrix CameraProjection::getProjectionMatrix(
    ProjectionType projectionType) const noexcept
{
    switch (projectionType) {
    case ProjectionType::Orthographic:
        return mtr::OrthographicProjection{width, height, zNear, zFar};
    case ProjectionType::Perspective:
        return mtr::PerspectiveProjectionWithAngle{degreeToRadian(angle),
                                                   width / height, zNear, zFar};
    }
    return mtr::Matrix::createIdentityMatrix();
}

} // namespace eng::ent
