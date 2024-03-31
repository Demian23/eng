#pragma once

#include "../../base/src/eng.h"
#include "../../matrix/src/Matrix.h"

namespace eng::ent {

enum class ProjectionType { Orthographic, Perspective };

class CameraProjection {
public:
    CameraProjection() noexcept = default;
    CameraProjection(floating w, floating h, floating zMin, floating zMax,
                     floating projectionAngle) noexcept
        : width{w}, height{h}, zNear{zMin}, zFar{zMax}, angle{projectionAngle}
    {}

    inline void reset(floating w, floating h, floating zMin, floating zMax,
                      floating projectionAngle) noexcept
    {
        width = w;
        height = h;
        zNear = zMin;
        zFar = zMax;
        angle = projectionAngle;
    }

    inline void setZComponent(floating zMin, floating zMax) noexcept
    {
        zNear = zMin;
        zFar = zMax;
    }

    [[nodiscard]] inline floating getAngleInDegrees() const noexcept
    {
        return angle;
    }

    inline void setAngleInDegrees(floating newAngle) noexcept
    {
        angle = newAngle;
    }

    [[nodiscard]] inline floating getAspect() const noexcept
    {
        return width / height;
    }

    [[nodiscard]] inline floating getZMin() const noexcept { return zNear; }

    [[nodiscard]] mtr::Matrix
    getProjectionMatrix(ProjectionType projectionType) const noexcept;

    [[nodiscard]] std::pair<eng::floating, eng::floating>
    getZComponent() const noexcept
    {
        return {zNear, zFar};
    }

private:
    floating width{}, height{};
    floating zNear{}, zFar{};
    floating angle{};
};

} // namespace eng::ent
