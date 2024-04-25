#pragma once

#include "../../algorithm/src/alg.h"
#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"
#include "../../entities/src/Model.h"
#include "../../pipeline/src/GraphicsPipeline.h"
#include "RGBArray.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <algorithm>
#include <format>
#include <ostream>

class ScreenDrawer : public Fl_Window {
public:
    ScreenDrawer(int width, int height, eng::ent::Model &&model,
                 eng::ent::Camera camera,
                 eng::ent::CameraProjection cameraProjection,
                 eng::ent::DistantLight light);
    void draw() override;
    int handle(int) override;

protected:
    eng::ent::Model _model;
    eng::ent::Camera _camera;
    eng::ent::CameraProjection _projection;
    eng::pipe::GraphicsPipeline _pipe;
    eng::ent::DistantLight _light;
    RGBArray screenArray;
    enum class Focused;
    enum class DrawStyle;
    Focused currentFocus;
    DrawStyle currentStyle;

    void printOutStatistic(std::ostream &stream);
};
