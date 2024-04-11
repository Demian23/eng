#pragma once
#include "../../base/src/Elements.h"
#include "../../entities/src/Light.h"
#include "../../matrix/src/Matrix.h"
#include <cstdint>

namespace eng::pipe {

// TODO copying too much

template <std::invocable<uint32_t, uint32_t, vec::Vec3F> Out>
class PhongShaderWithLambertColoring final {
public:
    using nci = std::vector<Normal>::const_iterator;

    PhongShaderWithLambertColoring(ent::DistantLight light, Out out,
                                   nci normals)
        : _light(light), _out(out), _normals(normals)
    {}

    void operator()(uint32_t x, uint32_t y, [[maybe_unused]] floating u,
                    [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                    Triangle triangle) const
    {
        auto aNormal = *(_normals + triangle[0].normalOffset);
        auto bNormal = *(_normals + triangle[1].normalOffset);
        auto cNormal = *(_normals + triangle[2].normalOffset);
        auto normalInPoint =
            Normal{aNormal[0] * u + bNormal[0] * v + cNormal[0] * w,
                   aNormal[1] * u + bNormal[1] * v + cNormal[1] * w,
                   aNormal[2] * u + bNormal[2] * v + cNormal[2] * w};
        auto product =
            std::max(0.f, normalInPoint * _light.direction.trim<3>());
        auto lightIntensity = product * _light.intensity;
        vec::Vec3F color{_light.color[0] * lightIntensity,
                         _light.color[1] * lightIntensity,
                         _light.color[2] * lightIntensity};
        _out(x, y, color);
    }

private:
    ent::DistantLight _light;
    Out _out;
    nci _normals;
};

} // namespace eng::pipe
