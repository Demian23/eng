#pragma once

#include "../../vector/src/DimensionalVector.h"

namespace eng::ent {
// TODO rewrite without dynamic polymorphism
class Light{
public:
    struct Intensities{
        vec::Vec3F ambient;
        vec::Vec3F diffuse;
        vec::Vec3F specular;
    };
    [[nodiscard]] virtual Intensities calculatePhongColor(vec::Vec3F inPoint, vec::Vec3F withNormal, vec::Vec3F albedo, vec::Vec3F specular, vec::Vec3F cameraPosition,   floating shine)const noexcept = 0;
    virtual ~Light() = default;
};
using LightArray = std::vector<std::unique_ptr<Light>>;

struct DistantLight final : Light{
public:
    [[nodiscard]] inline Intensities calculatePhongColor(vec::Vec3F inPoint, vec::Vec3F withNormal,  vec::Vec3F albedo, vec::Vec3F specular, vec::Vec3F cameraPosition, floating shine)const noexcept override{
        auto viewVector = (cameraPosition - inPoint).normalize();
        auto normalLightDot = withNormal * direction;
        if(normalLightDot > 0){
            auto reflectVector =
                (direction - withNormal * (2 * (normalLightDot)))
                    .normalize();
            auto shineDot = reflectVector * viewVector;
            auto specularValue = std::pow(shineDot, shine);
            return Intensities{
                {
                    color[0] * ambientIntensity * albedo[0],
                    color[1] * ambientIntensity * albedo[1],
                    color[2] * ambientIntensity * albedo[2]
                },
                {
                    color[0] * normalLightDot * albedo[0],
                    color[1] * normalLightDot * albedo[1],
                    color[2] * normalLightDot * albedo[2]
                },
                {
                    color[0] * specularValue * specular[0],
                    color[1] * specularValue * specular[1],
                    color[2] * specularValue * specular[2],
                }};
        } else {
            return Intensities{
                {
                    color[0] * ambientIntensity *albedo[0],
                    color[1] * ambientIntensity *albedo[1],
                    color[2] * ambientIntensity *albedo[2]
                }, {}, {}
            };
        }
    }
    ~DistantLight() override = default;
    DistantLight(vec::Vec3F d, vec::Vec3F c, floating i) : Light(), color(c), direction{d}, ambientIntensity(i){}
    vec::Vec3F color{0xFF, 0xFF, 0xFF};
    // to rotate mul on rotation matrix
    vec::Vec3F direction{0, 0, 1};
    floating ambientIntensity{0.2f};
};

struct PointLight final : Light{
    [[nodiscard]] inline Intensities calculatePhongColor(vec::Vec3F inPoint, vec::Vec3F withNormal,  vec::Vec3F albedo, vec::Vec3F specular, vec::Vec3F cameraPosition, floating shine)const noexcept override{
        auto direction = position - inPoint;
        auto distance = direction.length();
        auto attenuation = constantAttenuation + linearAttenuation * distance + expAttenuation * distance * distance;
        auto ambient = vec::Vec3F{
            color[0] * ambientIntensity * albedo[0],
            color[1] * ambientIntensity * albedo[1],
            color[2] * ambientIntensity * albedo[2]
        } / attenuation;
        direction.normalize();
        auto normalLightDot = withNormal * direction;
        if(normalLightDot > 0){
            auto viewVector = (cameraPosition - inPoint).normalize();
            auto reflectVector =
                (direction - withNormal * (2 * (normalLightDot)))
                    .normalize();
            auto shineDot = reflectVector * viewVector;
            auto specularValue = std::pow(shineDot, shine);
            auto diffuse =
                vec::Vec3F {
                    color[0] * normalLightDot * albedo[0],
                    color[1] * normalLightDot * albedo[1],
                    color[2] * normalLightDot * albedo[2]
                } / attenuation;
            auto spec =
            vec::Vec3F {
                color[0] * specularValue * specular[0],
                    color[1] * specularValue * specular[1],
                    color[2] * specularValue * specular[2],
            } / attenuation;
            return {ambient, diffuse, spec};
        } else {
            return {ambient, {}, {}};
        }
    }
    PointLight( vec::Vec3F p,vec::Vec3F c, floating i, floating ca, floating la, floating ea)
    : color(c), position(p), ambientIntensity(i), constantAttenuation(ca), linearAttenuation(la), expAttenuation(ea)
    {}
    ~PointLight() override = default;
    vec::Vec3F color{0xFF, 0xFF, 0xFF};
    vec::Vec3F position{0, 0, 1};
    floating ambientIntensity{0.2f};
    floating constantAttenuation;
    floating linearAttenuation;
    floating expAttenuation;
};


} // namespace eng::ent