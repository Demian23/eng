#pragma once

#include "../../matrix/src/Matrix.h"
#include "../../vector/src/DimensionalVector.h"

namespace eng::ent {

class Camera final {
public:
    Camera(vec::ThreeDimensionalVector cameraEye,
           vec::ThreeDimensionalVector cameraTarget,
           vec::ThreeDimensionalVector cameraUp) noexcept;

    [[nodiscard]] mtr::Matrix getViewMatrix() const noexcept;

    void rotateX(floating degree) noexcept;
    void rotateY(floating degree) noexcept;
    void rotateZ(floating degree) noexcept;

    void moveAlongDiagonal(floating movement) noexcept;
    void changePolarAngle(floating addition) noexcept;
    void changeAzimuthalAngle(floating addition) noexcept;

private:
    vec::FourDimensionalVector eye;
    vec::FourDimensionalVector target;
    vec::FourDimensionalVector up;

    void changeEyeAsSphericalVector(unsigned index, floating addition) noexcept;
};

} // namespace eng::ent
