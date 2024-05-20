#include "DeferredShaders.h"

namespace eng::dr {

eng::ScreenPixel DeferredPhong::operator()(Fragment fragment)
{
    if(fragment.albedo != vec::Vec3F {0, 0, 0}){
        vec::Vec3F diffuseIntensity{}, specularIntensity{}, ambientIntensity{};
        auto viewVector = (_cameraPosition - fragment.point).normalize();
        for(const auto& light : _lights){
            auto [color, lightDirection, intensity] = light->getColorIntensityAndDirection({}, {});
            auto normalLightDot = std::max(0.f, fragment.normal * lightDirection);
            auto reflectVector =
                (lightDirection - fragment.normal * (2 * (normalLightDot)))
                    .normalize();
            auto shineDot = reflectVector * viewVector;
            auto specularValue = std::pow(shineDot, _shine);
            diffuseIntensity += vec::Vec3F{
                color[0] * normalLightDot,
                color[1] * normalLightDot,
                color[2] * normalLightDot,
            };
            ambientIntensity += vec::Vec3F{
                color[0] * intensity,
                color[1] * intensity,
                color[2] * intensity,
            };
            specularIntensity += vec::Vec3F{
                color[0] * specularValue,
                color[1] * specularValue,
                color[2] * specularValue,
            };
        }
        vec::Vec3F resultColor{
            std::clamp(fragment.albedo[0] * (diffuseIntensity[0] + ambientIntensity[0]) +
                       specularIntensity[0] * fragment.specular[0],
                       0.f, 255.f),
            std::clamp(fragment.albedo[1] * (diffuseIntensity[1] + ambientIntensity[1]) +
                       specularIntensity[1] * fragment.specular[1],
                       0.f, 255.f),
            std::clamp(fragment.albedo[2] * (diffuseIntensity[2] + ambientIntensity[2]) +
                       specularIntensity[2] + fragment.specular[2],
                       0.f, 255.f),
        };
        return ScreenPixel{static_cast<uint8_t>(resultColor[0]),
                           static_cast<uint8_t>(resultColor[1]),
                           static_cast<uint8_t>(resultColor[2])};
    }
    return {0, 0, 0};
}

}; // namespace eng::dr
