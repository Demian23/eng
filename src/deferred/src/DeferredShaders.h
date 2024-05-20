#pragma once

#include "../../entities/src/Light.h"
#include "GBuffer.h"

namespace eng::dr {

class DeferredPhong {
public:
    DeferredPhong(const ent::LightArray& lights, vec::Vec3F cameraPosition,
                  floating shine)
        : _lights(lights), _cameraPosition(cameraPosition), _shine(shine)
    {}
    ScreenPixel operator()(Fragment fragment);

private:
    const ent::LightArray& _lights;
    vec::Vec3F _cameraPosition;
    floating _shine;
};

}; // namespace eng::dr
