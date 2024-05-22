#pragma once
#include "../../base/src/Elements.h"
#include "../../entities/src/Light.h"
#include "../../matrix/src/Matrix.h"
#include <cstdint>

namespace eng::shader {

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

class FullSpecularProperties {
public:
    FullSpecularProperties(vec::Vec3F eye, vci vertices, floating shine) : _eye(eye), _verticesInWorldSpace(vertices), _shine(shine){}
    inline auto operator()([[maybe_unused]] floating u,
                           [[maybe_unused]] floating v,
                           [[maybe_unused]] floating w,
                           [[maybe_unused]] Triangle triangle) const noexcept
    {
        return vec::Vec3F{1, 1, 1};
    }
    [[nodiscard]] vec::Vec3F getCameraPosition()const noexcept{return _eye;}
    [[nodiscard]] vci getVerticesInWorldSpace()const noexcept{return _verticesInWorldSpace;}
    [[nodiscard]] floating getShine()const noexcept{return _shine;}
private:
    vec::Vec3F _eye;
    vci _verticesInWorldSpace;
    floating _shine;
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

class TextureSpecular : public TextureAlbedo{
public:
    TextureSpecular(vci vertices, tci textures, const char *textureData,
        uint32_t width, uint32_t height, uint8_t channel, vec::Vec3F eye, vci verticesInWorldSpace, floating shine)
    : TextureAlbedo(vertices, textures, textureData, width, height,
        channel), _eye(eye), _verticesInWorldSpace(verticesInWorldSpace), _shine(shine)
    {}

    [[nodiscard]] vec::Vec3F getCameraPosition()const noexcept{return _eye;}
    [[nodiscard]] vci getVerticesInWorldSpace()const noexcept{return _verticesInWorldSpace;}
    [[nodiscard]] floating getShine()const noexcept{return _shine;}
private:
    vec::Vec3F _eye;
    vci _verticesInWorldSpace;
    floating _shine;

};

template <AlbedoProvider Albedo, NormalProvider Normal,
          SpecularPropertiesProvider Specular>
struct PhongShader {
    PhongShader(const ent::LightArray& lights, Albedo albedo, Normal normal,
           Specular specular)
        : _lights(lights), _albedo(albedo), _normal(normal), _specular(specular)
    {}

    [[nodiscard]] vec::Vec3F operator()([[maybe_unused]] floating u,
                    [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                    Triangle triangle) const noexcept{
        auto albedo = _albedo(u, v, w, triangle);
        auto normal = _normal(u, v, w, triangle);
        auto specularProperties= _specular(u, v, w, triangle);
        auto verticesInWorldSpace = _specular.getVerticesInWorldSpace();
        auto shine = _specular.getShine();
        auto cameraPosition = _specular.getCameraPosition();
        auto aInWorldSpace = *(verticesInWorldSpace + triangle[0].vertexOffset);
        auto bInWorldSpace = *(verticesInWorldSpace + triangle[1].vertexOffset);
        auto cInWorldSpace = *(verticesInWorldSpace + triangle[2].vertexOffset);
        auto point =
            vec::Vec3F{aInWorldSpace[0] * u + bInWorldSpace[0] * v +
                       cInWorldSpace[0] * w,
                       aInWorldSpace[1] * u + bInWorldSpace[1] * v +
                       cInWorldSpace[1] * w,
                       aInWorldSpace[2] * u + bInWorldSpace[2] * v +
                       cInWorldSpace[2] * w};
        vec::Vec3F diffuseIntensity{}, ambientIntensity{}, specularIntensity{};

        for(const auto& light : _lights){
            auto [ambient, diffuse, specular] = light->calculatePhongColor(point, normal, albedo, specularProperties, cameraPosition, shine);
            diffuseIntensity +=  diffuse;
            ambientIntensity += ambient;
            specularIntensity += specular;
        }

        return vec::Vec3F {
            std::clamp(diffuseIntensity[0] + ambientIntensity[0] +
                       specularIntensity[0],
                       0.f, 255.f),
            std::clamp(diffuseIntensity[1] + ambientIntensity[1] +
                       specularIntensity[1],
                       0.f, 255.f),
            std::clamp(diffuseIntensity[2] + ambientIntensity[2] +
                       specularIntensity[2],
                       0.f, 255.f),
        };
    }

private:
    const ent::LightArray& _lights;
    Albedo _albedo;
    Normal _normal;
    Specular _specular;
};


struct NoTexturePhongShader : PhongShader<ConstantAlbedo, NormalInterpolation,
                            FullSpecularProperties> {
    NoTexturePhongShader(const ent::LightArray& lights, vci verticesInWorldSpace,
                nci normalsInWorldSpace, vec::Vec3F albedo, vec::Vec3F eye,
                floating shine)
        : PhongShader<ConstantAlbedo, NormalInterpolation,
                 FullSpecularProperties>{
              lights,
              ConstantAlbedo{albedo},
              NormalInterpolation{normalsInWorldSpace},
              FullSpecularProperties{eye, verticesInWorldSpace, shine}}
    {}
};

struct TextureShader : PhongShader<TextureAlbedo, TextureNormal, TextureSpecular> {
    TextureShader(const ent::LightArray& lights, TextureAlbedo albedo,
                  TextureNormal normal, TextureSpecular specular)
        : PhongShader<TextureAlbedo, TextureNormal,
                 TextureSpecular>{
              lights,
              albedo,
              normal,
              specular}
    {}
};

struct FullSpecularWithTextureAlbedo
    : PhongShader<TextureAlbedo, NormalInterpolation,
             FullSpecularProperties> {
public:
    FullSpecularWithTextureAlbedo(const ent::LightArray& lights, TextureAlbedo albedo,
                                  vci verticesInWorldSpace,
                                  nci normalsInWorldSpace, vec::Vec3F eye,
                                  floating shine)
        : PhongShader<TextureAlbedo, NormalInterpolation,
                 FullSpecularProperties>{
              lights,
              albedo,
              NormalInterpolation{normalsInWorldSpace},
              FullSpecularProperties{eye, verticesInWorldSpace, shine}}
    {}
};

} // namespace eng::shader
