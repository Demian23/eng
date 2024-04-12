#pragma once
#include "../../base/src/Elements.h"
#include "../../entities/src/Light.h"
#include "../../matrix/src/Matrix.h"
#include <cstdint>

namespace eng::pipe {

template<typename T>
concept ShaderOut = std::invocable<T, uint32_t , uint32_t , vec::Vec3F>;

template <ShaderOut Out>
class FlatShaderWithLambertColoring final {
public:
    using vci = std::vector<Vertex>::const_iterator;

    FlatShaderWithLambertColoring(mtr::Matrix modelMatrix,
                                  ent::DistantLight light, Out out,
                                  vci vertices)
        : _modelMatrix(modelMatrix), _light(light), _out(out),
          _vertices(vertices)
    {}

    void operator()(uint32_t x, uint32_t y, [[maybe_unused]] floating u,
                    [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                    Triangle triangle) const
    {
        auto aInWorldSpace =
            (_modelMatrix * *(_vertices + triangle[0].vertexOffset)).trim<3>();
        auto bInWorldSpace =
            (_modelMatrix * *(_vertices + triangle[1].vertexOffset)).trim<3>();
        auto cInWorldSpace =
            (_modelMatrix * *(_vertices + triangle[2].vertexOffset)).trim<3>();
        auto tNormal = vec::cross(cInWorldSpace - aInWorldSpace,
                                  bInWorldSpace - aInWorldSpace)
            .normalize();
        auto product = std::max(0.f, tNormal * -(_light.direction.trim<3>()));
        auto lightIntensity = product * _light.intensity;
        vec::Vec3F color{_light.color[0] * lightIntensity,
                         _light.color[1] * lightIntensity,
                         _light.color[2] * lightIntensity};
        _out(x, y, color);
    }

private:
    mtr::Matrix _modelMatrix;
    ent::DistantLight _light;
    Out _out;
    vci _vertices;
};
// TODO copying too much

template <ShaderOut Out>
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
                   aNormal[2] * u + bNormal[2] * v + cNormal[2] * w}.normalize();
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
