#include "MonoColorDrawer.h"
#include "RGBArrayInserter.h"
#include "../../algorithm/src/alg.h"

MonoColorDrawer::MonoColorDrawer(int width, int height)
    : Fl_Window{width, height},
      screenArray{static_cast<uint64_t>(width * height), {0xFF, 0xFF, 0xFF}},
      backgroundColor{0xFF, 0xFF, 0xFF}, color{0, 0, 0}
{
    auto targetY = static_cast<float>(-(width - width* 0.1) / 2);
    eng::floating xMin = 0, xMax = xMin + static_cast<eng::floating>(width), yMin = 0,
        yMax = yMin + static_cast<eng::floating>(height), zMin = 0.1, zMax = 10;
    eng::vec::ThreeDimensionalVector eye{0, 0, 0},
        target{0, targetY, 1}, up{0, -1, 0}, scale{40, 40, 1};
    // how to insert here division on w
    currentTransformation =
        eng::mtr::Viewport{xMin, xMax, yMin, yMax} *
        eng::mtr::OrthographicProjection{xMin, xMax, yMin,
                                         yMax, zMin, zMax} *
        eng::mtr::View{eye, target, up} * eng::mtr::Move{target} *
        eng::mtr::Scale{scale};
}

void MonoColorDrawer::setNewPolygons(std::vector<eng::obj::PolygonVertexOnly> &&newPolygons)
{
    polygons = std::move(newPolygons);
    for (auto &polygon: polygons) {
        for(auto &vertex : polygon)
            vertex = currentTransformation * vertex;
    }
}

void MonoColorDrawer::draw()
{
    screenArray.fill(backgroundColor);
    if(!polygons.empty()){
        RGBArrayInserter inserter{screenArray, color, static_cast<uint64_t>(this->w())};
        for (const auto &polygon: polygons) {
            eng::alg::line<RGBArrayInserter::valueType>(
                static_cast<eng::numeric>(polygon[0][0]),
                static_cast<eng::numeric>((*polygon.rbegin())[0]),
                static_cast<eng::numeric>(polygon[0][1]),
                static_cast<eng::numeric>((*polygon.rbegin())[1]),
                inserter);
            for(unsigned i = 1; i < polygon.size(); i++){
                eng::alg::line<RGBArrayInserter::valueType>(
                    static_cast<eng::numeric>(polygon[i-1][0]),
                    static_cast<eng::numeric>(polygon[i][0]),
                    static_cast<eng::numeric>(polygon[i-1][1]),
                    static_cast<eng::numeric>(polygon[i][1]),
                    inserter);
            }
        }
    }
    fl_draw_image(screenArray.data(), 0, 0, w(), h(), 3);
}