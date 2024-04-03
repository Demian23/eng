#pragma once

#include "../../vector/src/DimensionalVector.h"

namespace eng::ent {

struct DistantLight {
    vec::Vec3F color{0xFF, 0xFF, 0xFF};
    // to rotate mul on rotation matrix
    vec::Vec4F direction{0, 0, 1, 0};
    floating intensity{1.f};
};

} // namespace eng::ent