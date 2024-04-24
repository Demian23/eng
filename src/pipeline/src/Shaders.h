#pragma once
#include "../../base/src/Elements.h"
#include "../../entities/src/Light.h"
#include "../../matrix/src/Matrix.h"
#include <cstdint>

namespace eng::pipe {

template<typename T>
concept ShaderOut = std::invocable<T, uint32_t , uint32_t , vec::Vec3F>;

using nci = std::vector<Normal>::const_iterator;
using vci = std::vector<Vertex>::const_iterator;
using tci = std::vector<TextureCoord>::const_iterator;

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


class NormalByPhong{
public:
    NormalByPhong(nci normals):_normals(normals){}

    inline auto operator()([[maybe_unused]] floating u,
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
       return normalInPoint;
    }
private:
    nci _normals;
};

template<typename SpecularProvider>
class SpecularByPhong{
public:
    SpecularByPhong(vec::Vec3F eye, vci vertices, floating shine, SpecularProvider provider) : _provider(provider), _eye(eye), _vertices(vertices), _shine(shine){}
    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                          [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                          Triangle triangle, vec::Vec3F lightDirection, vec::Vec3F normal) const
    {
        auto normalLightDot = std::max(0.f, normal * lightDirection);
        if(normalLightDot > 0){
            vec::Vec3F specularCoefficient = _provider(u, v, w, triangle);
            auto aInWorldSpace = *(_vertices + triangle[0].vertexOffset);
            auto bInWorldSpace = *(_vertices + triangle[1].vertexOffset);
            auto cInWorldSpace = *(_vertices + triangle[2].vertexOffset);
            auto point = vec::Vec3F {
                aInWorldSpace[0] * u + bInWorldSpace[0] * v + cInWorldSpace[0] * w,
                aInWorldSpace[1] * u + bInWorldSpace[1] * v + cInWorldSpace[1] * w,
                aInWorldSpace[2] * u + bInWorldSpace[2] * v + cInWorldSpace[2] * w
            };
            auto reflectVector = (lightDirection - normal * (2 * (normalLightDot))).normalize();
            auto viewVector = (_eye - point).normalize();
            auto shineDot = reflectVector * viewVector;
            auto specularValue = std::pow(shineDot, _shine);
            return {specularCoefficient[0] * specularValue, specularCoefficient[1] * specularValue, specularCoefficient[2] * specularValue};
        }
        return {0, 0, 0};
    }
private:
    SpecularProvider _provider;
    vec::Vec3F _eye;
    vci _vertices;
    floating _shine;
};

class TextureProcessing{
public:
    TextureProcessing(vci vertices, tci textures, const char* textureData, uint32_t  width, uint32_t height)
    : _vertices(vertices), _texturesCoordinates(textures), _textureData(textureData), _width(width), _height(height)
    {}

    [[nodiscard]] inline auto getRGB([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                                 Triangle triangle) const
    {
        auto aCoordinate= *(_texturesCoordinates + triangle[0].textureCoordinatesOffset);
        auto bCoordinate= *(_texturesCoordinates + triangle[1].textureCoordinatesOffset);
        auto cCoordinate= *(_texturesCoordinates + triangle[2].textureCoordinatesOffset);

        auto aX = aCoordinate[0] * static_cast<eng::floating>(_width) - static_cast<floating>(0.5);
        auto aY = (1 - aCoordinate[1]) * static_cast<eng::floating>(_height) - static_cast<floating>(0.5);
        auto bX = bCoordinate[0] * static_cast<eng::floating>(_width) - static_cast<floating>(0.5);
        auto bY = (1 - bCoordinate[1]) * static_cast<eng::floating>(_height) - static_cast<floating>(0.5);
        auto cX = cCoordinate[0] * static_cast<eng::floating>(_width) - static_cast<floating>(0.5);
        auto cY = (1 - cCoordinate[1]) * static_cast<eng::floating>(_height) - static_cast<floating>(0.5);

        auto aW = (*(_vertices + triangle[0].vertexOffset))[3];
        auto bW = (*(_vertices + triangle[1].vertexOffset))[3];
        auto cW = (*(_vertices + triangle[2].vertexOffset))[3];

        auto x = std::clamp(static_cast<uint32_t>((aX * u / aW  + bX * v / bW + cX * w / cW) / (u / aW + v  / bW + w / cW)), static_cast<uint32_t>(0), static_cast<uint32_t>(_width));
        auto y = std::clamp(static_cast<uint32_t>((aY * u / aW  + bY * v / bW + cY * w / cW) / (u / aW + v  / bW + w / cW)), static_cast<uint32_t>(0), static_cast<uint32_t>(_height));

        auto offset = static_cast<uint32_t>(((y * _width + x) * 3)); // maybe round
        auto rgbPtr = _textureData + offset;

        auto r = static_cast<eng::floating>(static_cast<uint8_t>(rgbPtr[0]));
        auto g = static_cast<eng::floating>(static_cast<uint8_t>(rgbPtr[1]));
        auto b = static_cast<eng::floating>(static_cast<uint8_t>(rgbPtr[2]));
        return vec::Vec3F {r, g, b};
    }
private:
    vci _vertices;
    tci _texturesCoordinates;
    const char* _textureData;
    uint32_t _width, _height;
};

class TextureAlbedo : private TextureProcessing {
public:
    TextureAlbedo(vci vertices, tci textures, const char* textureData, uint32_t  width, uint32_t height)
        : TextureProcessing(vertices, textures, textureData, width, height)
    {}

    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                    [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                    Triangle triangle) const
    {
        auto rgb = getRGB(u, v, w, triangle);
        return {rgb[0] / 0xFF, rgb[1] / 0xFF, rgb[2] / 0xFF};
    }
};

class TextureNormal : private TextureProcessing{
public:

    TextureNormal(vci vertices, mtr::Matrix modelMatrix, tci textures, const char* textureData, uint32_t width, uint32_t height)
        : TextureProcessing(vertices, textures, textureData, width, height), _modelMatrix(modelMatrix)
    {}

    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                                 Triangle triangle) const
    {
        auto rgb = getRGB(u, v, w, triangle);
        auto normal = vec::Vec4F {(rgb[0]  * 2 - 0xFF) / 0xFF, (rgb[1] * 2 - 0xFF) / 0xFF, (rgb[2]  * 2 - 0xFF)/ 0xFF, 0};
        return (_modelMatrix * normal).trim<3>().normalize();
    }
private:
    mtr::Matrix _modelMatrix;
};

using TextureSpecular = TextureAlbedo;

template<ShaderOut Out, typename AlbedoProvider, typename NormalProvider, typename SpecularProvider>
class PhongShader{
public:
    PhongShader(ent::DistantLight light, AlbedoProvider albedo, NormalProvider normal, SpecularProvider specular, Out out)
        : _light(light), _albedo(albedo), _normal(normal), _specular(specular), _out(out)
    {}

    void operator()(uint32_t x, uint32_t y, [[maybe_unused]] floating u,
                    [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                    Triangle triangle) const
    {
        auto albedo = _albedo(u, v, w, triangle);
        auto normal = _normal(u, v, w, triangle);
        auto color = _light.color;
        auto lightDirection = _light.direction.trim<3>();
        auto lightIntensity = _light.intensity;
        auto specular = _specular(u, v, w, triangle, lightDirection, normal);

        auto normalLightDot = std::max(0.f, normal * lightDirection);
        auto diffuseIntensity =  vec::Vec3F {
            color[0] * albedo[0] * normalLightDot,
            color[1] * albedo[1] * normalLightDot,
            color[2] * albedo[2] * normalLightDot,
        };
        auto ambientIntensity = vec::Vec3F {
            color[0] * albedo[0] * lightIntensity,
            color[1] * albedo[1] * lightIntensity,
            color[2] * albedo[2] * lightIntensity,
        };
        auto specularIntensity = vec::Vec3F {
            color[0] * specular[0],
            color[1] * specular[1],
            color[2] * specular[2],
        };
        vec::Vec3F resultColor{
            std::clamp(diffuseIntensity[0] + specularIntensity[0] + ambientIntensity[0], 0.f, 255.f),
            std::clamp(diffuseIntensity[1] + specularIntensity[1] + ambientIntensity[1], 0.f, 255.f),
            std::clamp(diffuseIntensity[2] + specularIntensity[2] + ambientIntensity[2], 0.f, 255.f),
        };
        _out(x, y, resultColor);
    }

private:
    ent::DistantLight _light;
    AlbedoProvider _albedo;
    NormalProvider _normal;
    SpecularProvider _specular;
    Out _out;
};

} // namespace eng::pipe
