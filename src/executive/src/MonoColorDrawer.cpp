#include "MonoColorDrawer.h"
#include "RGBArrayInserter.h"
#include "../../algorithm/src/alg.h"
#include <FL/fl_draw.H>

eng::mtr::Matrix MonoColorDrawer::createNewTransformationMatrix(
    eng::mtr::Matrix additionalMatrix = eng::mtr::Matrix::createIdentityMatrix())
{
    auto targetY = static_cast<float>(-(h()- h()* 0.1) / 2);
    eng::floating xMin = 0, xMax = xMin + static_cast<eng::floating>(w()), yMin = 0,
        yMax = yMin + static_cast<eng::floating>(h()), zMin = 0.1, zMax = 10;
    eng::vec::ThreeDimensionalVector eye{0, 0, 0},
        target{0, targetY, 1}, up{0, -1, 0};
    // how to insert here division on w
    currentTransformation =
        eng::mtr::Viewport{xMin, xMax, yMin, yMax} *
        eng::mtr::OrthographicProjection{xMin, xMax, yMin,
                                         yMax, zMin, zMax} *
        eng::mtr::View{eye, target, up} * eng::mtr::Move{target} *
        additionalMatrix;
    return currentTransformation;
}

MonoColorDrawer::MonoColorDrawer(int width, int height)
    : Fl_Window{width, height}, currentTransformation(createNewTransformationMatrix()),
      screenArray{static_cast<uint64_t>(width * height), {0xFF, 0xFF, 0xFF}},
      backgroundColor{0xFF, 0xFF, 0xFF}, color{0, 0, 0}
{
}

std::vector<eng::obj::PolygonVertexOnly> MonoColorDrawer::applyCurrentTransformation()
{
    auto copy = polygons;
    for (auto &polygon: copy) {
        for(auto &vertex : polygon)
            vertex = currentTransformation * vertex;
    }
    return copy;
}


void MonoColorDrawer::setNewPolygons(std::vector<eng::obj::PolygonVertexOnly> &&newPolygons)
{
    polygons = std::move(newPolygons);
}

void MonoColorDrawer::draw()
{
    screenArray.fill(backgroundColor);
    if(!polygons.empty()){
        auto copy = applyCurrentTransformation();
        RGBArrayInserter inserter{screenArray, color, static_cast<uint64_t>(this->w())};
        for (const auto &polygon: copy) {
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


int MonoColorDrawer::handle(int event){
    enum ProcessedKeys{RotateX = 'x', RotateZ = 'z', RotateY = 'y', Scale = 's', MoveX = 'r', MoveY = 'u', MoveZ = 'f'};
    if(event == FL_SHORTCUT) {
        switch (Fl::event_key()) {
        case RotateX:
        {
            auto degree = Fl::event_shift() ? -2 : 2;
            changeModel(eng::mtr::RotateX{degree});
            break;
        }
        case RotateY:
        {
            auto degree = Fl::event_shift() ? -2 : 2;
            changeModel(eng::mtr::RotateY{degree});
            break;
        }
        case RotateZ:
        {
            auto degree = Fl::event_shift() ? -2 : 2;
            changeModel(eng::mtr::RotateZ{degree});
            break;
        }
        case Scale:
        {
            eng::floating degree = Fl::event_shift() ? 0.5 : 2;
            changeModel(eng::mtr::Scale{{degree, degree, degree}});
            break;
        }
        case MoveX:
        {
            eng::floating addition = static_cast<eng::floating>(w()) * 0.05f;
            addition = Fl::event_shift() ? -addition : addition;
            changeModel(eng::mtr::Move{{addition, 0, 0}});
            break;
        }
        case MoveY:
        {
            eng::floating addition = static_cast<eng::floating>(h()) * 0.05f;
            addition = Fl::event_shift() ? -addition : addition;
            changeModel(eng::mtr::Move{{0, addition, 0}});
            break;
        }
        case MoveZ:
        {
            eng::floating addition = static_cast<eng::floating>(10) * 0.05f;
            addition = Fl::event_shift() ? -addition : addition;
            changeModel(eng::mtr::Move{{0, addition, 0}});
            break;
        }
        default:
            return Fl_Window::handle(event);
        }
        redraw();
        return 1;
    }
    return Fl_Window::handle(event);
}

void MonoColorDrawer::changeModel(eng::mtr::Matrix rotationMatrix)
{
    for (auto &polygon: polygons) {
        for(auto &vertex : polygon)
            vertex = rotationMatrix * vertex;
    }
}

