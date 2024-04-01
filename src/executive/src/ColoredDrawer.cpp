#include "ColoredDrawer.h"
#include "RGBArrayInterpolator.h"
#include "../../rasterization/src/rast.h"

ColoredDrawer::ColoredDrawer(int width, int height, eng::ent::Model &&model,
              eng::ent::Camera camera,
              eng::ent::CameraProjection cameraProjection) : ScreenDrawer(width, height, std::move(model), camera, cameraProjection){}

void ColoredDrawer::draw()
{
    screenArray.fill({0, 0, 0});
    auto copy = _pipe.applyVertexTransformations(0, w(), 0, h());
    auto copyIterator = copy.cbegin();
    RGB a_color = RGB{125, 0, 0};
    RGB b_color = RGB{255, 0, 0};
    RGB c_color = RGB{125, 0, 0};
    std::for_each(
        _pipe.trianglesBegin(), _pipe.trianglesEnd(),
        [=, inserter = RGBArrayInterpolator{screenArray, a_color, b_color, c_color,
                                        static_cast<uint32_t>(w())}](
            auto &&polygon) mutable {
            eng::rast::barycentricPoint((copyIterator + polygon[0].vertexOffset)->template trim<2>(), (copyIterator + polygon[1].vertexOffset)->template trim<2>(),(copyIterator + polygon[2].vertexOffset)->template trim<2>(), inserter);
        });
    fl_draw_image(screenArray.data(), 0, 0, w(), h());
}
