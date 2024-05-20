#include "DeferredShaders.h"

namespace eng::dr {

eng::ScreenPixel DeferredPhong::operator()(Fragment fragment)
{
    auto lightDirection = _light.direction.trim<3>();
    auto normalLightDot = std::max(0.f, fragment.normal * lightDirection);
    if (normalLightDot > 0) {

        auto reflectVector =
            (lightDirection - fragment.normal * (2 * (normalLightDot)))
                .normalize();
        auto viewVector = (_cameraPosition - fragment.point).normalize();
        auto shineDot = reflectVector * viewVector;
        auto specularValue = std::pow(shineDot, _shine);
        auto specular = Specular{fragment.specular[0] * specularValue,
                                 fragment.specular[1] * specularValue,
                                 fragment.specular[2] * specularValue};
        auto diffuseIntensity = vec::Vec3F{
            _light.color[0] * fragment.albedo[0] * normalLightDot,
            _light.color[1] * fragment.albedo[1] * normalLightDot,
            _light.color[2] * fragment.albedo[2] * normalLightDot,
        };
        auto ambientIntensity = vec::Vec3F{
            _light.color[0] * fragment.albedo[0] * _light.intensity,
            _light.color[1] * fragment.albedo[1] * _light.intensity,
            _light.color[2] * fragment.albedo[2] * _light.intensity,
        };
        auto specularIntensity = vec::Vec3F{
            _light.color[0] * specular[0],
            _light.color[1] * specular[1],
            _light.color[2] * specular[2],
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
        return ScreenPixel{static_cast<uint8_t>(resultColor[0]),
                           static_cast<uint8_t>(resultColor[1]),
                           static_cast<uint8_t>(resultColor[2])};
    }
    return {0, 0, 0};
}

}; // namespace eng::dr
