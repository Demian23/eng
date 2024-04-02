#pragma once
#include "ScreenDrawer.h"

class ColoredDrawer : public ScreenDrawer {
public:
    ColoredDrawer(int width, int height, eng::ent::Model &&model,
                  eng::ent::Camera camera,
                  eng::ent::CameraProjection cameraProjection);
    void draw() override;
};
