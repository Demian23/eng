#include "DeferredShaders.h"

namespace eng::dr {

eng::ScreenPixel DeferredPhong::operator()(Fragment fragment)
{
    if(fragment.albedo != vec::Vec3F {0, 0, 0}){
        vec::Vec3F diffuseIntensity{}, specularIntensity{}, ambientIntensity{};
        for(const auto& light : _lights){
            auto [ambient, diffuse, specular] = light->calculatePhongColor(fragment.point, fragment.normal,  fragment.albedo, fragment.specular, _cameraPosition, _shine);
            diffuseIntensity += diffuse;
            ambientIntensity += ambient;
            specularIntensity += specular;
        }
        vec::Vec3F resultColor{
            std::clamp(diffuseIntensity[0] + ambientIntensity[0] +
                       specularIntensity[0] ,
                       0.f, 255.f),
            std::clamp(diffuseIntensity[1] + ambientIntensity[1] +
                       specularIntensity[1],
                       0.f, 255.f),
            std::clamp(diffuseIntensity[2] + ambientIntensity[2] +
                       specularIntensity[2],
                       0.f, 255.f),
        };
        return ScreenPixel{static_cast<uint8_t>(resultColor[0]),
                           static_cast<uint8_t>(resultColor[1]),
                           static_cast<uint8_t>(resultColor[2])};
    }
    return {0, 0, 0};
}

}; // namespace eng::dr
