#pragma once

#include "../../algorithm/src/alg.h"
#include "RGBArray.h"
#include "RGBArrayInserter.h"
#include "../../matrix/src/Viewport.h"
#include "../../entities/src/Model.h"
#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <algorithm>

template <typename PolygonType> 
class MonoColoredScreenDrawer final : public Fl_Window {
public:
    MonoColoredScreenDrawer(int width, int height,
            eng::ent::Model<PolygonType>&& aModel, 
            eng::ent::Camera aCamera,
            eng::ent::CameraProjection aCameraProjection)
        : Fl_Window{width, height}, 
          model{std::move(aModel)}, camera{aCamera},
          cameraProjection{aCameraProjection},
          screenArray{static_cast<uint64_t>(width * height)}
    {}

    [[nodiscard]] std::vector<PolygonType> applyTransformations() const noexcept
    {
        using namespace eng;
        using namespace eng::mtr;
        using namespace eng::vec;
        using namespace std;

        std::vector<PolygonType> copy{model.getPolygonsBegin(), model.getPolygonsEnd()};
        
        auto transformationMatrix = Viewport{0, 0 + static_cast<floating>(w()), 0, 0 + static_cast<floating>(h())} * cameraProjection.getProjectionMatrix(projectionType) * camera.getViewMatrix() * model.getModelMatrix();
        auto transformVertex =  
                    [transformationMatrix, projection = this->projectionType](auto&& vertex){
                        vertex = transformationMatrix * vertex;
                    if (projection == eng::ent::ProjectionType::Perspective) {
                        transform(vertex.begin(), vertex.end(), vertex.begin(),
                                  [w = *vertex.rbegin()](auto &&coord) {
                                      return w != 0 ? coord /= w : coord;
                                  });
                    }
                    return vertex;
                    };
        for(auto &polygon : copy){
            transform(polygon.begin(), polygon.end(), polygon.begin(), 
                    transformVertex);
        }
        return copy;
    }

    void draw() override
    {
        screenArray.fill(backgroundColor);
        if (!model.empty()) {
            auto copy = applyTransformations();
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
        if(event == FL_SHORTCUT && Fl::event_key() == 's'){
            eng::floating degree = Fl::event_shift() ? 0.5 : 2;
            model.scale(degree);
            redraw();
            return 1;
        }
        return Fl_Window::handle(event);
    }

    inline void setBackgroundColor(RGB newColor) { backgroundColor = newColor; }
    inline void setColor(RGB newColor) { color = newColor; }
private:
    eng::ent::Model<PolygonType> model;
    eng::ent::Camera camera;
    eng::ent::CameraProjection cameraProjection;
    RGBArray screenArray;
    RGB backgroundColor{0xFF, 0xFF, 0xFF}, color{0, 0, 0};
    eng::ent::ProjectionType projectionType{eng::ent::ProjectionType::Orthographic};
};
