#pragma once

#include "../../eng.h"
#include "../../matrix/src/Matrix.h"

namespace eng::ent {

class CameraProjection {
public:
    enum class ProjectionType{Orthographic, Perspective};
    CameraProjection(floating w, floating h, floating zMin, floating zMax, floating angle);
    [[nodiscard]]
    inline floating getAngleInDegrees()const noexcept{return angle;}
    inline void setAngleInDegrees(floating newAngle){angle = newAngle;}
    [[nodiscard]]
    mtr::Matrix getProjectionMatrix(ProjectionType projectionType) const;
private:
    floating width, height;
    floating zNear, zFar;
    floating angle;
};

} // namespace eng::ent
