#pragma once

#include "../../entities/src/Light.h"
#include "GBuffer.h"

namespace eng::dr {

class DeferredPhong {
public:
    DeferredPhong(ent::DistantLight light, vec::Vec3F cameraPosition,
                  floating shine)
        : _light(light), _cameraPosition(cameraPosition), _shine(shine)
    {}
    ScreenPixel operator()(Fragment fragment);

private:
    ent::DistantLight _light;
    vec::Vec3F _cameraPosition;
    floating _shine;
};

}; // namespace eng::dr
