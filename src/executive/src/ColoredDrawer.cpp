#include "ColoredDrawer.h"

ColoredDrawer::ColoredDrawer(int width, int height, eng::ent::Model &&model,
                             eng::ent::Camera camera,
                             eng::ent::CameraProjection cameraProjection)
    : ScreenDrawer(width, height, std::move(model), camera, cameraProjection)
{
    _pipe.setZBufferSize(static_cast<uint32_t>(width * height),
                         static_cast<uint32_t>(w()));
}

void ColoredDrawer::draw()
{
    screenArray.fill({0, 0, 0});
    RGB _aColor = RGB{125, 0, 0};
    RGB _bColor = RGB{255, 0, 0};
    RGB _cColor = RGB{125, 0, 0};
    auto xSize = static_cast<uint32_t>(w());
    auto colorInserter = [=, this](uint32_t x, uint32_t y, eng::floating u,
                                   eng::floating v, eng::floating w) {
        RGB color{};
        color.r =
            static_cast<uint8_t>(u * static_cast<eng::floating>(_aColor.r) +
                                 v * static_cast<eng::floating>(_bColor.r) +
                                 w * static_cast<eng::floating>(_cColor.r));
        color.b =
            static_cast<uint8_t>(u * static_cast<eng::floating>(_aColor.b) +
                                 v * static_cast<eng::floating>(_bColor.b) +
                                 w * static_cast<eng::floating>(_cColor.b));
        color.g =
            static_cast<uint8_t>(u * static_cast<eng::floating>(_aColor.g) +
                                 v * static_cast<eng::floating>(_bColor.g) +
                                 w * static_cast<eng::floating>(_cColor.g));
        screenArray[y * xSize + x] = color;
    };
    _pipe.rasterModel(0, w(), 0, h(), colorInserter);
    fl_draw_image(screenArray.data(), 0, 0, w(), h());
}
