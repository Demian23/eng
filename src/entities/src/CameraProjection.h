#pragma once

#include "../../base/src/eng.h"
#include "../../matrix/src/Matrix.h"

namespace eng::ent {

enum class ProjectionType { Orthographic, Perspective };

class CameraProjection {
public:
    CameraProjection(floating w, floating h, floating zMin, floating zMax,
                     floating projectionAngle) noexcept
        : width{w}, height{h}, zNear{zMin}, zFar{zMax}, angle{projectionAngle}
    {}

    [[nodiscard]] inline floating getAngleInDegrees() const noexcept
    {
        return angle;
    }

    inline void setAngleInDegrees(floating newAngle) noexcept { angle = newAngle; }

    [[nodiscard]] mtr::Matrix
    getProjectionMatrix(ProjectionType projectionType) const noexcept;

private:
    floating width, height;
    floating zNear, zFar;
    floating angle;
};

} // namespace eng::ent
