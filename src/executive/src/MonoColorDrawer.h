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

    // TODO: target, polygons and color are one creature
    MonoColorDrawer(int width, int height,
                    eng::vec::ThreeDimensionalVector cameraEye,
                    eng::vec::ThreeDimensionalVector targetPosition = {0, 0, 0})
        : Fl_Window{width, height}, polygons{},
          screenArray{static_cast<uint64_t>(width * height),
                      {0xFF, 0xFF, 0xFF}},
          backgroundColor{0xFF, 0xFF, 0xFF}, color{0, 0, 0}, eye{cameraEye},
          target{targetPosition}
    {}

    [[nodiscard]] std::vector<PolygonType> applyTransformation() const noexcept
    {
        using namespace eng;
        using namespace eng::mtr;
        using namespace eng::vec;
        using namespace std;
        floating xMin = 0, xMax = xMin + static_cast<floating>(w()), yMin = 0,
                 yMax = yMin + static_cast<floating>(h()), zMin = 0.1,
                 zMax = 10;
        auto angleInRad =
            degreeToRadian(90.0); // TODO: user wanna change that value
        auto aspect = (xMax - xMin) / (yMax - yMin);

        auto copy = polygons;

        // TODO: user wanna change projection in runtime
        auto projection =
            PerspectiveProjectionWithAngle{angleInRad, aspect, zMin, zMax};
        // OrthographicProjection{xMin, xMax, yMin, yMax, zMin, zMax};
        // PerspectiveProjection{xMin, xMax, yMin, yMax, zMin, zMax}
        auto transformations =
            Viewport{xMin, xMax, yMin, yMax} * projection *
            View{eng::vec::sphericalToCartesian(eye), target};

        for (auto &polygon : copy)
            transform(
                polygon.begin(), polygon.end(), polygon.begin(),
                [transformations](auto &&vertex) {
                    vertex = transformations * vertex;
                    if constexpr (is_same_v<decltype(projection),
                                            PerspectiveProjectionWithAngle> ||
                                  is_same_v<decltype(projection),
                                            PerspectiveProjection>) {
                        transform(vertex.begin(), vertex.end(), vertex.begin(),
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
    eng::vec::ThreeDimensionalVector target;
};
