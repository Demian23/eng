#pragma once

#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"
#include "../../entities/src/Light.h"
#include "../../entities/src/Model.h"
#include "../../entities/src/PixelArray.h"
#include "../../pipeline/src/GraphicsPipeline.h"
#include <FL/Fl_Window.H>

class ScreenDrawer : public Fl_Window {
public:
    ScreenDrawer(int width, int height, eng::ent::Model &&model,
                 eng::ent::Camera camera,
                 eng::ent::CameraProjection cameraProjection,
                 eng::ent::LightArray&& lights);
    void draw() override;
    int handle(int) override;

protected:
    eng::ent::Model _model;
    eng::ent::Camera _camera;
    eng::ent::CameraProjection _projection;
    eng::pipe::GraphicsPipeline _pipe;
    eng::ent::LightArray _lights;
    eng::ent::PixelArray screenArray;
    enum class Focused;
    enum class DrawStyle;
    Focused currentFocus;
    DrawStyle currentStyle;
};
