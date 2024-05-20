#pragma once

#include "../../vector/src/DimensionalVector.h"

namespace eng::ent {
// TODO rewrite without dynamic polymorphism
class Light{
public:
    struct CID{
        vec::Vec3F color;
        vec::Vec3F direction;
        floating intensity;
    };
    [[nodiscard]] virtual CID getColorIntensityAndDirection(vec::Vec3F inPoint, vec::Vec3F withNormal) const noexcept = 0;
    virtual ~Light() = default;
};
using LightArray = std::vector<std::unique_ptr<Light>>;

struct DistantLight final : Light{
public:
    [[nodiscard]] inline CID getColorIntensityAndDirection([[maybe_unused]]vec::Vec3F inPoint, [[maybe_unused]]vec::Vec3F withNormal) const noexcept override{
        return {color, direction.trim<3>(), intensity};
    }
    ~DistantLight() override = default;
    DistantLight(vec::Vec3F d, vec::Vec3F c, floating i) : Light(), color(c), direction{d[0], d[1], d[2], 0}, intensity(i){}
    vec::Vec3F color{0xFF, 0xFF, 0xFF};
    // to rotate mul on rotation matrix
    vec::Vec4F direction{0, 0, 1, 0};
    floating intensity{1.f};
};

} // namespace eng::ent