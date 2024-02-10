#include "MonoColorDrawer.h"
#include "../../algorithm/src/alg.h"
#include "RGBArrayInserter.h"
#include <FL/fl_draw.H>

eng::mtr::Matrix MonoColorDrawer::createNewTransformationMatrix(int width, int height) noexcept
{
   auto targetY = static_cast<eng::floating>(-(height - height * 0.1) / 2);
    eng::floating xMin = 0, xMax = xMin + static_cast<eng::floating>(width),
                  yMin = 0, yMax = yMin + static_cast<eng::floating>(height),
                  zMin = 0.1, zMax = 1000;
    eng::vec::ThreeDimensionalVector eye{0, 0, 0}, target{0, targetY, 1},
        up{0, -1, 0};
    // how to insert here division on w
    auto result =
        eng::mtr::Viewport{xMin, xMax, yMin, yMax} *
        eng::mtr::OrthographicProjection{xMin, xMax, yMin, yMax, zMin, zMax} *
        eng::mtr::View{eye, target, up} * eng::mtr::Move{target};
    return result;
}

MonoColorDrawer::MonoColorDrawer(int width, int height)
    : Fl_Window{width, height},
      screenArray{static_cast<uint64_t>(width * height), {0xFF, 0xFF, 0xFF}},
      backgroundColor{0xFF, 0xFF, 0xFF}, color{0, 0, 0}
{}

std::vector<eng::obj::PolygonVertexOnly>
MonoColorDrawer::applyTransformation()const noexcept
{
    auto copy = polygons;
    auto transformation = createNewTransformationMatrix(w(), h());
    for (auto &polygon : copy) {
        std::transform(polygon.begin(), polygon.end(), polygon.begin(),
                       [transformation](eng::obj::Vertex vertex) {
                           return transformation * vertex;
                       });
    }
    return copy;
}

void MonoColorDrawer::setNewPolygons(
    std::vector<eng::obj::PolygonVertexOnly> &&newPolygons)
{
    polygons = std::move(newPolygons);
}

void MonoColorDrawer::draw()
{
    screenArray.fill(backgroundColor);
    if (!polygons.empty()) {
        auto copy = applyTransformation();
        RGBArrayInserter inserter{screenArray, color,
                                  static_cast<uint64_t>(this->w())};
        for (const auto &polygon : copy) {
            eng::alg::line(static_cast<eng::numeric>(polygon[0][0]),
                           static_cast<eng::numeric>((*polygon.rbegin())[0]),
                           static_cast<eng::numeric>(polygon[0][1]),
                           static_cast<eng::numeric>((*polygon.rbegin())[1]),
                           inserter);
            for (unsigned i = 1; i < polygon.size(); i++) {
                eng::alg::line(static_cast<eng::numeric>(polygon[i - 1][0]),
                               static_cast<eng::numeric>(polygon[i][0]),
                               static_cast<eng::numeric>(polygon[i - 1][1]),
                               static_cast<eng::numeric>(polygon[i][1]),
                               inserter);
            }
        }
    }
    fl_draw_image(screenArray.data(), 0, 0, w(), h(), 3);
}

int MonoColorDrawer::handle(int event)
{
    enum ProcessedKeys {
        RotateX = 'x',
        RotateZ = 'z',
        RotateY = 'y',
        Scale = 's',
        MoveX = 'r',
        MoveY = 'u',
        MoveZ = 'f'
    };
    if (event == FL_SHORTCUT) {
        switch (Fl::event_key()) {
        case RotateX: {
            auto degree = Fl::event_shift() ? -2 : 2;
            changeModel(eng::mtr::RotateX{degree});
            break;
        }
        case RotateY: {
            auto degree = Fl::event_shift() ? -2 : 2;
            changeModel(eng::mtr::RotateY{degree});
            break;
        }
        case RotateZ: {
            auto degree = Fl::event_shift() ? -2 : 2;
            changeModel(eng::mtr::RotateZ{degree});
            break;
        }
        case Scale: {
            eng::floating degree = Fl::event_shift() ? 0.5 : 2;
            changeModel(eng::mtr::Scale{{degree, degree, degree}});
            break;
        }
        case MoveX: {
            eng::floating addition = static_cast<eng::floating>(w()) * 0.05f;
            addition = Fl::event_shift() ? -addition : addition;
            changeModel(eng::mtr::Move{{addition, 0, 0}});
            break;
        }
        case MoveY: {
            eng::floating addition = static_cast<eng::floating>(h()) * 0.05f;
            addition = Fl::event_shift() ? -addition : addition;
            changeModel(eng::mtr::Move{{0, addition, 0}});
            break;
        }
        case MoveZ: {
            eng::floating addition = static_cast<eng::floating>(1000) * 0.05f;
            addition = Fl::event_shift() ? -addition : addition;
            changeModel(eng::mtr::Move{{0, 0, addition}});
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

void MonoColorDrawer::changeModel(eng::mtr::Matrix matrix)
{
    for (auto &polygon : polygons) {
        std::transform(polygon.begin(), polygon.end(), polygon.begin(), [matrix](auto vertex){return matrix * vertex;});
    }
}
