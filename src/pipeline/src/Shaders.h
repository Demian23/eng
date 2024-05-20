#pragma once
#include "../../base/src/Elements.h"
#include "../../entities/src/Light.h"
#include "../../matrix/src/Matrix.h"
#include <cstdint>

namespace eng::shader {

template <typename T>
concept ShaderOut =
    std::invocable<T, uint32_t, uint32_t, vec::Vec3F> && requires(T out) {
        {
            out({}, {}, {})
        } -> std::same_as<void>;
    };

template <typename T>
concept AlbedoProvider =
    std::invocable<T, floating, floating, floating, Triangle> &&
    requires(T provider) {
        {
            provider({}, {}, {}, {})
        } -> std::convertible_to<vec::Vec3F>;
    };

template <typename T>
concept NormalProvider =
    std::invocable<T, floating, floating, floating, Triangle> &&
    requires(T provider) {
        {
            provider({}, {}, {}, {})
        } -> std::convertible_to<vec::Vec3F>;
    };
template <typename T>
concept SpecularPropertiesProvider =
    std::invocable<T, floating, floating, floating, Triangle> &&
    requires(T provider) {
        {
            provider({}, {}, {}, {})
        } -> std::convertible_to<vec::Vec3F>;
    };
template <typename T>
concept SpecularProvider = std::invocable<T, floating, floating, floating,
                                          Triangle, vec::Vec3F, vec::Vec3F> &&
                           requires(T provider) {
                               {
                                   provider({}, {}, {}, {}, {}, {})
                               } -> std::convertible_to<vec::Vec3F>;
                           };

using nci = std::vector<Normal>::const_iterator;
using vci = std::vector<Vertex>::const_iterator;
using tci = std::vector<TextureCoord>::const_iterator;

struct NormalInterpolation {
    explicit NormalInterpolation(nci normals) : _normals(normals) {}

    vec::Vec3F operator()([[maybe_unused]] floating u,
                          [[maybe_unused]] floating v,
                          [[maybe_unused]] floating w,
                          Triangle triangle) const noexcept;

private:
    nci _normals;
};

struct NormalForTriangle {
    explicit NormalForTriangle(vci verticesInWorldSpace)
        : _vertices(verticesInWorldSpace)
    {}
    vec::Vec3F operator()(Triangle triangle) const noexcept;

private:
    vci _vertices;
};

struct NormalForTriangleWithCaching : private NormalForTriangle {
    explicit NormalForTriangleWithCaching(vci verticesInWorldSpace)
        : NormalForTriangle(verticesInWorldSpace), _previousTriangle{},
          _previousNormal{NormalForTriangle::operator()(_previousTriangle)}
    {}
    vec::Vec3F operator()([[maybe_unused]] floating u,
                          [[maybe_unused]] floating v,
                          [[maybe_unused]] floating w,
                          Triangle triangle) const noexcept;

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

struct FullSpecularProperties {
    inline auto operator()([[maybe_unused]] floating u,
                           [[maybe_unused]] floating v,
                           [[maybe_unused]] floating w,
                           [[maybe_unused]] Triangle triangle) const noexcept
    {
        return vec::Vec3F{1, 1, 1};
    }
};

struct ConstantAlbedo {
    explicit ConstantAlbedo(vec::Vec3F albedo) : _albedo(albedo) {}

    inline vec::Vec3F
    operator()([[maybe_unused]] floating u, [[maybe_unused]] floating v,
               [[maybe_unused]] floating w,
               [[maybe_unused]] Triangle triangle) const noexcept
    {
        return _albedo;
    }

private:
    vec::Vec3F _albedo;
};

template <SpecularPropertiesProvider Properties> struct SpecularCalculation {
    SpecularCalculation(vec::Vec3F eye, vci vertices, floating shine,
                        Properties provider)
        : _provider(provider), _eye(eye), _vertices(vertices), _shine(shine)
    {}
    vec::Vec3F operator()([[maybe_unused]] floating u,
                          [[maybe_unused]] floating v,
                          [[maybe_unused]] floating w, Triangle triangle,
                          vec::Vec3F lightDirection, vec::Vec3F normal) const
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

struct TextureProcessing {
    TextureProcessing(vci vertices, tci textures, const char *textureData,
                      uint32_t width, uint32_t height, uint8_t channel)
        : _vertices(vertices), _texturesCoordinates(textures),
          _textureData(textureData), _width(width), _height(height),
          _channel(channel)
    {}

    [[nodiscard]] vec::Vec3<uint8_t> getRGB([[maybe_unused]] floating u,
                                            [[maybe_unused]] floating v,
                                            [[maybe_unused]] floating w,
                                            Triangle triangle) const noexcept;

private:
    vci _vertices;
    tci _texturesCoordinates;
    const char *_textureData;
    uint32_t _width, _height;
    uint8_t _channel;
};

struct TextureAlbedo : private TextureProcessing {
    TextureAlbedo(vci vertices, tci textures, const char *textureData,
                  uint32_t width, uint32_t height, uint8_t channel)
        : TextureProcessing(vertices, textures, textureData, width, height,
                            channel)
    {}

    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v,
                                 [[maybe_unused]] floating w,
                                 Triangle triangle) const noexcept
    {
        auto rgb = getRGB(u, v, w, triangle);
        return {static_cast<floating>(rgb[0]) / 0xFF,
                static_cast<floating>(rgb[1]) / 0xFF,
                static_cast<floating>(rgb[2]) / 0xFF};
    }
};

struct TextureNormal : private TextureProcessing {
    TextureNormal(mtr::Matrix modelMatrix, vci vertices, tci textures,
                  const char *textureData, uint32_t width, uint32_t height,
                  uint8_t channel)
        : TextureProcessing(vertices, textures, textureData, width, height,
                            channel),
          _modelMatrix(modelMatrix)
    {}

    inline vec::Vec3F operator()([[maybe_unused]] floating u,
                                 [[maybe_unused]] floating v,
                                 [[maybe_unused]] floating w,
                                 Triangle triangle) const noexcept
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
struct Shader {
    Shader(const ent::LightArray& lights, Albedo albedo, Normal normal,
           Specular specular, Out out)
        : _lights(lights), _albedo(albedo), _normal(normal), _specular(specular),
          _out(out)
    {}

    void operator()(uint32_t x, uint32_t y, [[maybe_unused]] floating u,
                    [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                    Triangle triangle) const noexcept
    {
        auto albedo = _albedo(u, v, w, triangle);
        auto normal = _normal(u, v, w, triangle);
        floating normalLightDot = 0.f;
        vec::Vec3F diffuseIntensity{}, ambientIntensity{}, specularIntensity{};
        for(const auto& light : _lights){
            auto [color, lightDirection, lightIntensity] = light->getColorIntensityAndDirection({}, {});
            auto specular= _specular(u, v, w, triangle, lightDirection, normal);
            normalLightDot = std::max(0.f, normal * lightDirection);
            diffuseIntensity += vec::Vec3F{
                color[0] * normalLightDot,
                color[1] * normalLightDot,
                color[2] * normalLightDot,
            };
            ambientIntensity += vec::Vec3F{
                color[0] * lightIntensity,
                color[1] * lightIntensity,
                color[2] * lightIntensity,
            };
            specularIntensity += vec::Vec3F{
                color[0] * specular[0],
                color[1] * specular[1],
                color[2] * specular[2],
            };
        }

        vec::Vec3F resultColor{
            std::clamp(albedo[0]*(diffuseIntensity[0] + ambientIntensity[0]) +
                           specularIntensity[0],
                       0.f, 255.f),
            std::clamp(albedo[1] * (diffuseIntensity[1] + ambientIntensity[1]) +
                           specularIntensity[1],
                       0.f, 255.f),
            std::clamp(albedo[2] * (diffuseIntensity[2] + ambientIntensity[2]) +
                           specularIntensity[2],
                       0.f, 255.f),
        };
        _out(x, y, resultColor);
    }

private:
    const ent::LightArray& _lights;
    Albedo _albedo;
    Normal _normal;
    Specular _specular;
    Out _out;
};

template <ShaderOut Out>
struct LambertShader
    : Shader<Out, ConstantAlbedo, NormalForTriangleWithCaching, NoSpecular> {
    LambertShader(const ent::LightArray& lights, vci verticesInWorldSpace,
                  vec::Vec3F albedo, Out out)
        : Shader<Out, ConstantAlbedo, NormalForTriangleWithCaching, NoSpecular>{
              lights, // TODO invert direction
              ConstantAlbedo{albedo},
              NormalForTriangleWithCaching{verticesInWorldSpace},
              NoSpecular{},
              out}
    {}
};

template <ShaderOut Out>
struct PhongShader : Shader<Out, ConstantAlbedo, NormalInterpolation,
                            SpecularCalculation<FullSpecularProperties>> {
    PhongShader(const ent::LightArray& lights, vci verticesInWorldSpace,
                nci normalsInWorldSpace, vec::Vec3F albedo, vec::Vec3F eye,
                floating shine, Out out)
        : Shader<Out, ConstantAlbedo, NormalInterpolation,
                 SpecularCalculation<FullSpecularProperties>>{
              lights,
              ConstantAlbedo{albedo},
              NormalInterpolation{normalsInWorldSpace},
              {eye, verticesInWorldSpace, shine, FullSpecularProperties{}},
              out}
    {}
};

template <ShaderOut Out>
struct TextureShader : Shader<Out, TextureAlbedo, TextureNormal,
                              SpecularCalculation<TextureSpecular>> {
    TextureShader(const ent::LightArray& lights, TextureAlbedo albedo,
                  TextureNormal normal, TextureSpecular specular,
                  vci verticesInWorldSpace, vec::Vec3F eye, floating shine,
                  Out out)
        : Shader<Out, TextureAlbedo, TextureNormal,
                 SpecularCalculation<TextureSpecular>>{
              lights,
              albedo,
              normal,
              {eye, verticesInWorldSpace, shine, specular},
              out}
    {}
};

template <ShaderOut Out>
struct FullSpecularWithTextureAlbedo
    : Shader<Out, TextureAlbedo, NormalInterpolation,
             SpecularCalculation<FullSpecularProperties>> {
public:
    FullSpecularWithTextureAlbedo(const ent::LightArray& lights, TextureAlbedo albedo,
                                  vci verticesInWorldSpace,
                                  nci normalsInWorldSpace, vec::Vec3F eye,
                                  floating shine, Out out)
        : Shader<Out, TextureAlbedo, NormalInterpolation,
                 SpecularCalculation<FullSpecularProperties>>{
              lights,
              albedo,
              NormalInterpolation{normalsInWorldSpace},
              {eye, verticesInWorldSpace, shine, FullSpecularProperties{}},
              out}
    {}
};

} // namespace eng::shader
