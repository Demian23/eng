#pragma once

#include "../../base/src/eng.h"
#include "../../matrix/src/Matrix.h"

namespace eng::ent {

enum class ProjectionType { Orthographic, Perspective };

class CameraProjection {
public:
    CameraProjection() noexcept = default;
    CameraProjection(floating w, floating h, floating zMin = 0.01f,
                     floating zMax = 100.f,
                     floating projectionAngle = 90) noexcept;

    void reset(floating w, floating h, floating zMin, floating zMax,
               floating projectionAngle) noexcept;

    void setZComponent(floating zMin, floating zMax) noexcept;

    [[nodiscard]] floating getAngleInDegrees() const noexcept;

    void setAngleInDegrees(floating newAngle) noexcept;

    [[nodiscard]] floating getAspect() const noexcept;

    [[nodiscard]] floating getZMin() const noexcept;

    [[nodiscard]] mtr::Matrix
    getProjectionMatrix(ProjectionType projectionType) const noexcept;

    [[nodiscard]] std::pair<eng::floating, eng::floating>
    getZComponent() const noexcept;

private:
    floating width{}, height{};
    floating zNear{}, zFar{};
    floating angle{};
};

} // namespace eng::ent
