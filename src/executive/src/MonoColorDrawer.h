#pragma once

#include "../../algorithm/src/alg.h"
#include "../../matrix/src/Move.h"
#include "../../matrix/src/OrthographicProjection.h"
#include "../../matrix/src/PerspectiveProjection.h"
#include "../../matrix/src/PerspectiveProjectionWithAngle.h"
#include "../../matrix/src/Rotate.h"
#include "../../matrix/src/Scale.h"
#include "../../matrix/src/View.h"
#include "../../matrix/src/Viewport.h"
#include "../../objparser/src/PolygonVertexOnly.h"
#include "MonoColorDrawer.h"
#include "RGBArray.h"
#include "RGBArrayInserter.h"
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <algorithm>

template <typename PolygonType> class MonoColorDrawer final : public Fl_Window {
public:
    inline void setBackgroundColor(RGB newColor) { backgroundColor = newColor; }
    inline void setColor(RGB newColor) { color = newColor; }

    MonoColorDrawer(int width, int height, eng::vec::ThreeDimensionalVector cameraEye)
        : Fl_Window{width, height},  polygons{}, eye{cameraEye},
          screenArray{static_cast<uint64_t>(width * height),
                      {0xFF, 0xFF, 0xFF}},
          backgroundColor{0xFF, 0xFF, 0xFF}, color{0, 0, 0}
    {}

    [[nodiscard]] std::vector<PolygonType> applyTransformation() const noexcept
    {
        eng::floating xMin = 0, xMax = xMin + static_cast<eng::floating>(w()),
                      yMin = 0, yMax = yMin + static_cast<eng::floating>(h()),
                      zMin = 0.1, zMax = 10;
        auto angleInRad = eng::mtr::degree_to_rad(90);
        auto aspect = (xMax - xMin) / (yMax - yMin);
        eng::vec::ThreeDimensionalVector target{0, 0, 0};

        auto copy = polygons;

        auto projection =
             eng::mtr::PerspectiveProjectionWithAngle{angleInRad, aspect,
             zMin, zMax};
            //eng::mtr::OrthographicProjection{xMin, xMax, yMin,
            //                                 yMax, zMin, zMax};
        // eng::mtr::PerspectiveProjection{xMin, xMax, yMin, yMax, zMin, zMax}
        auto transformations = eng::mtr::Viewport{xMin, xMax, yMin, yMax} * projection * eng::mtr::View{eng::vec::sphericalToCartesian(eye), target};
        for (auto &polygon : copy)
            std::transform(
                polygon.begin(), polygon.end(), polygon.begin(),
                [transformations](auto &&vertex) {
                    vertex = transformations * vertex;
                    if constexpr (
                        std::is_same_v<
                            decltype(projection),
                            eng::mtr::PerspectiveProjectionWithAngle> ||
                        std::is_same_v<decltype(projection),
                                       eng::mtr::PerspectiveProjection>) {
                        std::transform(vertex.begin(), vertex.end(),
                                       vertex.begin(),
                                       [w = *vertex.rbegin()](auto &&coord) {
                                           return w != 0 ? coord /= w : coord;
                                       });
                    }
                    return vertex;
                });
        return copy;
    }

    void setNewPolygons(std::vector<PolygonType> &&newPolygons)
    {
        polygons = std::move(newPolygons);
    }

    void draw() override
    {
        screenArray.fill(backgroundColor);
        if (!polygons.empty()) {
            auto copy = applyTransformation();
            RGBArrayInserter inserter{screenArray, color,
                                      static_cast<uint64_t>(this->w())};
            for (const auto &polygon : copy) {
                eng::alg::line(
                    static_cast<eng::numeric>(polygon[0][0]),
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

    int handle(int event) override
    {
        enum ProcessedKeys {
            MoveCameraX = 'p',
            MoveCameraY = 'o',
            MoveCameraZ = 'i',
            RotateCameraY = 'q',
            RotateCameraX = 'w',
            RotateCameraZ = 'e',
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
            case RotateCameraX:
                eye = eng::vec::homogeneousToCartesian(
                    eng::mtr::RotateX{3} *
                    eng::vec::cartesianToHomogeneous(eye, 0));
                break;
            case RotateCameraY:
                eye = eng::vec::homogeneousToCartesian(
                    eng::mtr::RotateY{3} *
                    eng::vec::cartesianToHomogeneous(eye, 0));
                break;
            case RotateCameraZ:
                eye = eng::vec::homogeneousToCartesian(
                    eng::mtr::RotateZ{3} *
                    eng::vec::cartesianToHomogeneous(eye, 0));
                break;
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
                eng::floating addition =
                    static_cast<eng::floating>(w()) * 0.05f;
                addition = Fl::event_shift() ? -addition : addition;
                changeModel(eng::mtr::Move{{addition, 0, 0}});
                break;
            }
            case MoveY: {
                eng::floating addition =
                    static_cast<eng::floating>(h()) * 0.05f;
                addition = Fl::event_shift() ? -addition : addition;
                changeModel(eng::mtr::Move{{0, addition, 0}});
                break;
            }
            case MoveZ: {
                eng::floating addition =
                    static_cast<eng::floating>(1000) * 0.05f;
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

    void changeModel(eng::mtr::Matrix matrix)
    {
        for (auto &polygon : polygons) {
            std::transform(polygon.begin(), polygon.end(), polygon.begin(),
                           [matrix](auto &&vertex) { return matrix * vertex; });
        }
    }

private:
    std::vector<PolygonType> polygons;
    RGBArray screenArray;
    RGB backgroundColor, color;
    eng::vec::ThreeDimensionalVector eye;
};
