#pragma once
#include "../../base/src/Elements.h"
#include "../../entities/src/Light.h"
#include "../../matrix/src/Matrix.h"
#include <cstdint>

namespace eng::shader {

template <typename T>
concept ShaderOut = std::invocable<T, uint32_t, uint32_t, vec::Vec3F> && requires(T out){{out({}, {}, {})} -> std::same_as<void>;};

template<typename T>
concept AlbedoProvider = std::invocable<T, floating, floating, floating, Triangle>
                         && requires(T provider){ { provider({}, {}, {}, {}) } -> std::convertible_to<vec::Vec3F>;};

template<typename T>
concept NormalProvider = std::invocable<T, floating, floating, floating, Triangle>
                         && requires(T provider){ { provider({}, {}, {}, {}) } -> std::convertible_to<vec::Vec3F>;};
template<typename T>
concept SpecularPropertiesProvider = std::invocable<T, floating, floating, floating, Triangle>
                           && requires(T provider){ { provider({}, {}, {}, {}) } -> std::convertible_to<vec::Vec3F>;};
template<typename T>
concept SpecularProvider = std::invocable<T, floating, floating, floating, Triangle, vec::Vec3F, vec::Vec3F>
                           && requires(T provider){ { provider({}, {}, {}, {}, {}, {}) } -> std::convertible_to<vec::Vec3F>;};


using nci = std::vector<Normal>::const_iterator;
using vci = std::vector<Vertex>::const_iterator;
using tci = std::vector<TextureCoord>::const_iterator;

class NormalInterpolation{
public:
    NormalInterpolation(nci normals) : _normals(normals) {}

    inline auto operator()([[maybe_unused]] floating u,
                           [[maybe_unused]] floating v,
                           [[maybe_unused]] floating w, Triangle triangle) const noexcept
    {
        auto aNormal = *(_normals + triangle[0].normalOffset);
        auto bNormal = *(_normals + triangle[1].normalOffset);
        auto cNormal = *(_normals + triangle[2].normalOffset);
        auto normalInPoint =
            Normal{aNormal[0] * u + bNormal[0] * v + cNormal[0] * w,
                   aNormal[1] * u + bNormal[1] * v + cNormal[1] * w,
                   aNormal[2] * u + bNormal[2] * v + cNormal[2] * w}
                .normalize();
        return normalInPoint;
    }

private:
    nci _normals;
};

class NormalForTriangle {
public:
    NormalForTriangle(vci verticesInWorldSpace)
        : _vertices(verticesInWorldSpace)
    {}

    inline auto operator()(Triangle triangle) const noexcept
    {
        auto a =(*(_vertices + triangle[0].vertexOffset)).trim<3>();
        auto b = (*(_vertices + triangle[1].vertexOffset)).trim<3>();
        auto c= (*(_vertices + triangle[2].vertexOffset)).trim<3>();
        return vec::cross(c - a, b - a).normalize();
    }

private:
    vci _vertices;
};

class NormalForTriangleWithCaching : private NormalForTriangle{
public:
    NormalForTriangleWithCaching(vci verticesInWorldSpace)  : NormalForTriangle(verticesInWorldSpace), _previousTriangle{}, _previousNormal{NormalForTriangle::operator()(_previousTriangle)}
    {}
    inline auto operator()([[maybe_unused]] floating u,
                           [[maybe_unused]] floating v,
                           [[maybe_unused]] floating w, Triangle triangle) const noexcept
    {
       if(_previousTriangle == triangle){
           return _previousNormal;
       }else{
           _previousTriangle = triangle;
           _previousNormal = NormalForTriangle::operator()(_previousTriangle);
           return _previousNormal;
       }
    }
private:
    mutable Triangle _previousTriangle;
    mutable vec::Vec3F _previousNormal;
};

struct NoSpecular {
    inline auto operator()([[maybe_unused]] floating, [[maybe_unused]] floating,
                           [[maybe_unused]] floating, [[maybe_unused]] Triangle,
                           [[maybe_unused]] vec::Vec3F,
                           [[maybe_unused]] vec::Vec3F) const noexcept
    {
        return vec::Vec3F{0, 0, 0};
    }
};

struct FullSpecularProperties{
    inline auto operator()([[maybe_unused]] floating u,
                                       [[maybe_unused]] floating v,
                                       [[maybe_unused]] floating w,[[maybe_unused]] Triangle triangle) const noexcept{
        return vec::Vec3F{1, 1, 1};
    }
};

class ConstantAlbedo{
public:
    ConstantAlbedo(vec::Vec3F albedo) : _albedo(albedo){}

    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v,
                                 [[maybe_unused]] floating w,
                                 [[maybe_unused]] Triangle triangle) const noexcept{return _albedo;}
private:
    vec::Vec3F _albedo;
};


template <SpecularPropertiesProvider Properties>
class SpecularCalculation{
public:
    SpecularCalculation(vec::Vec3F eye, vci vertices, floating shine,
                    Properties provider)
        : _provider(provider), _eye(eye), _vertices(vertices), _shine(shine)
    {}
    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v,
                                 [[maybe_unused]] floating w, Triangle triangle,
                                 vec::Vec3F lightDirection,
                                 vec::Vec3F normal) const
    {
        auto normalLightDot = std::max(0.f, normal * lightDirection);
        if (normalLightDot > 0) {
            vec::Vec3F specularCoefficient = _provider(u, v, w, triangle);
            auto aInWorldSpace = *(_vertices + triangle[0].vertexOffset);
            auto bInWorldSpace = *(_vertices + triangle[1].vertexOffset);
            auto cInWorldSpace = *(_vertices + triangle[2].vertexOffset);
            auto point =
                vec::Vec3F{aInWorldSpace[0] * u + bInWorldSpace[0] * v +
                               cInWorldSpace[0] * w,
                           aInWorldSpace[1] * u + bInWorldSpace[1] * v +
                               cInWorldSpace[1] * w,
                           aInWorldSpace[2] * u + bInWorldSpace[2] * v +
                               cInWorldSpace[2] * w};
            auto reflectVector =
                (lightDirection - normal * (2 * (normalLightDot))).normalize();
            auto viewVector = (_eye - point).normalize();
            auto shineDot = reflectVector * viewVector;
            auto specularValue = std::pow(shineDot, _shine);
            return {specularCoefficient[0] * specularValue,
                    specularCoefficient[1] * specularValue,
                    specularCoefficient[2] * specularValue};
        }
        return {0, 0, 0};
    }

private:
    Properties _provider;
    vec::Vec3F _eye;
    vci _vertices;
    floating _shine;
};

class TextureProcessing {
public:
    TextureProcessing(vci vertices, tci textures, const char *textureData,
                      uint32_t width, uint32_t height, uint8_t channel)
        : _vertices(vertices), _texturesCoordinates(textures),
          _textureData(textureData), _width(width), _height(height),
          _channel(channel)
    {}

    [[nodiscard]] inline auto getRGB([[maybe_unused]] floating u,
                                     [[maybe_unused]] floating v,
                                     [[maybe_unused]] floating w,
                                     Triangle triangle) const
    {
        auto aCoordinate =
            *(_texturesCoordinates + triangle[0].textureCoordinatesOffset);
        auto bCoordinate =
            *(_texturesCoordinates + triangle[1].textureCoordinatesOffset);
        auto cCoordinate =
            *(_texturesCoordinates + triangle[2].textureCoordinatesOffset);

        auto aX = aCoordinate[0] * static_cast<floating>(_width) -
                  static_cast<floating>(0.5);
        auto aY = (1 - aCoordinate[1]) * static_cast<floating>(_height) -
                  static_cast<floating>(0.5);
        auto bX = bCoordinate[0] * static_cast<floating>(_width) -
                  static_cast<floating>(0.5);
        auto bY = (1 - bCoordinate[1]) * static_cast<floating>(_height) -
                  static_cast<floating>(0.5);
        auto cX = cCoordinate[0] * static_cast<floating>(_width) -
                  static_cast<floating>(0.5);
        auto cY = (1 - cCoordinate[1]) * static_cast<floating>(_height) -
                  static_cast<floating>(0.5);

        auto aW = (*(_vertices + triangle[0].vertexOffset))[3];
        auto bW = (*(_vertices + triangle[1].vertexOffset))[3];
        auto cW = (*(_vertices + triangle[2].vertexOffset))[3];

        auto x = std::clamp(
            static_cast<uint64_t>((aX * u / aW + bX * v / bW + cX * w / cW) /
                                  (u / aW + v / bW + w / cW)),
            static_cast<uint64_t>(0), static_cast<uint64_t>(_width));
        auto y = std::clamp(
            static_cast<uint64_t>((aY * u / aW + bY * v / bW + cY * w / cW) /
                                  (u / aW + v / bW + w / cW)),
            static_cast<uint64_t>(0), static_cast<uint64_t>(_height));

        auto offset =
            static_cast<uint64_t>(((y * _width + x) * _channel)); // maybe round
        auto rgbPtr = _textureData + offset;

        auto r = static_cast<uint8_t>(rgbPtr[0]);
        auto g = static_cast<uint8_t>(rgbPtr[1]);
        auto b = static_cast<uint8_t>(rgbPtr[2]);
        return vec::Vec3<uint8_t>{r, g, b};
    }

private:
    vci _vertices;
    tci _texturesCoordinates;
    const char *_textureData;
    uint32_t _width, _height;
    uint8_t _channel;
};

class TextureAlbedo : private TextureProcessing {
public:
    TextureAlbedo(vci vertices, tci textures, const char *textureData,
                  uint32_t width, uint32_t height, uint8_t channel)
        : TextureProcessing(vertices, textures, textureData, width, height,
                            channel)
    {}

    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v,
                                 [[maybe_unused]] floating w,
                                 Triangle triangle) const
    {
        auto rgb = getRGB(u, v, w, triangle);
        return {static_cast<floating>(rgb[0]) / 0xFF,
                static_cast<floating>(rgb[1]) / 0xFF,
                static_cast<floating>(rgb[2]) / 0xFF};
    }
};

class TextureNormal : private TextureProcessing {
public:
    TextureNormal(mtr::Matrix modelMatrix, vci vertices,  tci textures,
                  const char *textureData, uint32_t width, uint32_t height,
                  uint8_t channel)
        : TextureProcessing(vertices, textures, textureData, width, height,
                            channel),
          _modelMatrix(modelMatrix)
    {}

    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v,
                                 [[maybe_unused]] floating w,
                                 Triangle triangle) const
    {
        auto rgb = getRGB(u, v, w, triangle);
        auto normal =
            vec::Vec4F{(static_cast<floating>(rgb[0]) / 0xFF) * 2 - 1,
                       (static_cast<floating>(rgb[1]) / 0xFF) * 2 - 1,
                       (static_cast<floating>(rgb[2]) / 0xFF) * 2 - 1, 0};
        return (_modelMatrix * normal).normalize().trim<3>();
    }

private:
    mtr::Matrix _modelMatrix;
};

using TextureSpecular = TextureAlbedo;

template <ShaderOut Out, AlbedoProvider Albedo, NormalProvider Normal,
          SpecularProvider Specular>
class Shader {
public:
    Shader(ent::DistantLight light, Albedo albedo,
                Normal normal, Specular specular, Out out)
        : _light(light), _albedo(albedo), _normal(normal), _specular(specular),
          _out(out)
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
        auto diffuseIntensity = vec::Vec3F{
            color[0] * albedo[0] * normalLightDot,
            color[1] * albedo[1] * normalLightDot,
            color[2] * albedo[2] * normalLightDot,
        };
        auto ambientIntensity = vec::Vec3F{
            color[0] * albedo[0] * lightIntensity,
            color[1] * albedo[1] * lightIntensity,
            color[2] * albedo[2] * lightIntensity,
        };
        auto specularIntensity = vec::Vec3F{
            color[0] * specular[0],
            color[1] * specular[1],
            color[2] * specular[2],
        };
        vec::Vec3F resultColor{
            std::clamp(diffuseIntensity[0] + specularIntensity[0] +
                           ambientIntensity[0],
                       0.f, 255.f),
            std::clamp(diffuseIntensity[1] + specularIntensity[1] +
                           ambientIntensity[1],
                       0.f, 255.f),
            std::clamp(diffuseIntensity[2] + specularIntensity[2] +
                           ambientIntensity[2],
                       0.f, 255.f),
        };
        _out(x, y, resultColor);
    }

private:
    ent::DistantLight _light;
    Albedo _albedo;
    Normal _normal;
    Specular _specular;
    Out _out;
};

template <ShaderOut Out>
class LambertShader : public Shader<Out, ConstantAlbedo, NormalForTriangleWithCaching, NoSpecular>{
public:
    LambertShader(ent::DistantLight light, vci verticesInWorldSpace, vec::Vec3F albedo, Out out)
        : Shader<Out, ConstantAlbedo, NormalForTriangleWithCaching, NoSpecular>{{light.color, -light.direction, light.intensity}, {albedo}, {verticesInWorldSpace}, NoSpecular{}, out}
    {}
};

template <ShaderOut Out>
class PhongShader : public Shader<Out, ConstantAlbedo, NormalInterpolation, SpecularCalculation<FullSpecularProperties>>{
public:
    PhongShader(ent::DistantLight light, vci verticesInWorldSpace, nci normalsInWorldSpace, vec::Vec3F albedo, vec::Vec3F eye, floating shine, Out out)
        : Shader<Out, ConstantAlbedo, NormalInterpolation, SpecularCalculation<FullSpecularProperties>>{
                                             light, {albedo}, {normalsInWorldSpace}, {eye, verticesInWorldSpace, shine, FullSpecularProperties{}}, out}
    {}
};

template <ShaderOut Out>
class TextureShader: public Shader<Out, TextureAlbedo, TextureNormal, SpecularCalculation<TextureSpecular>>{
public:
    TextureShader(ent::DistantLight light, TextureAlbedo albedo, TextureNormal normal, TextureSpecular specular, vci verticesInWorldSpace, vec::Vec3F eye, floating shine, Out out)
        : Shader<Out, TextureAlbedo, TextureNormal, SpecularCalculation<TextureSpecular>>{
        light, albedo, normal, {eye, verticesInWorldSpace, shine, specular}, out}
    {}
};

} // namespace eng::pipe
