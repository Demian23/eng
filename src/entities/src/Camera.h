#pragma once

#include "../../matrix/src/Matrix.h"
#include "../../vector/src/DimensionalVector.h"

namespace eng::ent {

class Camera final {
public:
    Camera() noexcept = default;
    Camera(vec::Vec3F cameraEye, vec::Vec3F cameraTarget,
           vec::Vec3F cameraUp) noexcept;

    [[nodiscard]] mtr::Matrix getViewMatrix() const noexcept;

    void reset(vec::Vec3F cameraEye, vec::Vec3F cameraTarget,
               vec::Vec3F cameraUp) noexcept;

    void rotateX(floating degree) noexcept;
    void rotateY(floating degree) noexcept;
    void rotateZ(floating degree) noexcept;

    void moveAlongDiagonal(floating movement) noexcept;
    void changePolarAngle(floating addition) noexcept;
    void changeAzimuthalAngle(floating addition) noexcept;

    void moveTarget(vec::Vec3F position) noexcept;

    [[nodiscard]] inline eng::floating getDiagonalLength() const noexcept
    {
        return eye.length();
    }

    [[nodiscard]] inline const vec::Vec3F getEye() const noexcept
    {
        return vec::homogeneousToCartesian(eye);
    }
    [[nodiscard]] inline vec::Vec3F getTarget() const noexcept
    {
        return vec::homogeneousToCartesian(target);
    }

private:
    vec::Vec4F eye;
    vec::Vec4F target;
    vec::Vec4F up;

    void changeEyeAsSphericalVector(unsigned index, floating addition) noexcept;
};

} // namespace eng::ent
