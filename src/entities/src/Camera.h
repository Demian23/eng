#pragma once

#include "../../matrix/src/Matrix.h"
#include "../../vector/src/DimensionalVector.h"

namespace eng::ent {

// TODO: remove inline and check that in assembly it's not function call
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

    [[nodiscard]] eng::floating getDiagonalLength() const noexcept;

    [[nodiscard]] vec::Vec3F getEye() const noexcept;

    [[nodiscard]] vec::Vec3F getTarget() const noexcept;

private:
    vec::Vec4F eye;
    vec::Vec4F target;
    vec::Vec4F up;

    void changeEyeAsSphericalVector(unsigned index, floating addition) noexcept;
};

} // namespace eng::ent
