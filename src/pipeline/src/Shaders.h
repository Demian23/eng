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

template <ShaderOut Out>
class PhongShaderWithPhongColoring final {
public:
    using nci = std::vector<Normal>::const_iterator;
    using vci = std::vector<Vertex>::const_iterator;

    PhongShaderWithPhongColoring(
        ent::DistantLight light,
        Out out,
        vci vertices,
        nci normals,
        vec::Vec3F albedo,
        floating shine, vec::Vec3F eye)
        : _light(light), _out(out), _vertices(vertices), _normals(normals),  _eye(eye), _albedo(albedo), _shine(shine)
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
        auto defaultLightIntensity = _light.intensity;
        auto color = _light.color;
        auto lightDirection3 = _light.direction.trim<3>();
        auto normalLightDot = std::max(0.f, normalInPoint * lightDirection3);
        auto diffuseIntensity=  vec::Vec3F {
            color[0] * _albedo[0] * normalLightDot,
            color[1] * _albedo[1] * normalLightDot,
            color[2] * _albedo[2] * normalLightDot,
        };

        auto ambientIntensity = vec::Vec3F {
            color[0] * _albedo[0] * defaultLightIntensity,
            color[1] * _albedo[1] * defaultLightIntensity,
            color[2] * _albedo[2] * defaultLightIntensity,
        };


        auto specularIntensity = vec::Vec3F {0, 0, 0};
        if (normalLightDot > 0) {
            auto aInWorldSpace = *(_vertices + triangle[0].vertexOffset);
            auto bInWorldSpace = *(_vertices + triangle[1].vertexOffset);
            auto cInWorldSpace = *(_vertices + triangle[2].vertexOffset);
            auto point = vec::Vec3F {
                aInWorldSpace[0] * u + bInWorldSpace[0] * v + cInWorldSpace[0] * w,
                aInWorldSpace[1] * u + bInWorldSpace[1] * v + cInWorldSpace[1] * w,
                aInWorldSpace[2] * u + bInWorldSpace[2] * v + cInWorldSpace[2] * w
            };
            auto reflectVector = (lightDirection3 - normalInPoint * (2 * (normalLightDot))).normalize();
            auto viewVector = (_eye - point).normalize();
            auto shineDot = reflectVector * viewVector;
            auto specularValue = std::pow(shineDot, _shine);
            specularIntensity  = vec::Vec3F{color[0] * specularValue, color[1] * specularValue, color[2] * specularValue};
        }

        vec::Vec3F resultColor{
            std::clamp(diffuseIntensity[0] + specularIntensity[0] + ambientIntensity[0], 0.f, 255.f),
            std::clamp(diffuseIntensity[1] + specularIntensity[1] + ambientIntensity[1], 0.f, 255.f),
            std::clamp(diffuseIntensity[2] + specularIntensity[2] + ambientIntensity[2], 0.f, 255.f),
        };
        _out(x, y, resultColor);
    }

private:
    ent::DistantLight _light;
    Out _out;
    vci _vertices;
    nci _normals;
    vec::Vec3F _eye, _albedo;
    floating _shine;
};

} // namespace eng::pipe
