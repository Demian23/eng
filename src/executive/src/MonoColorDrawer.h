#pragma once

#include "../../matrix/src/Move.h"
#include "../../matrix/src/OrthographicProjection.h"
#include "../../matrix/src/Rotate.h"
#include "../../matrix/src/Scale.h"
#include "../../matrix/src/View.h"
#include "../../matrix/src/Viewport.h"
#include "../../objparser/src/PolygonVertexOnly.h"
#include "RGBArray.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>

class MonoColorDrawer final : public Fl_Window {
public:
    MonoColorDrawer(int width, int height);
    inline void setBackgroundColor(RGB newColor) { backgroundColor = newColor; }
    inline void setColor(RGB newColor) { color = newColor; }
    void setNewPolygons(std::vector<eng::obj::PolygonVertexOnly> &&newPolygons);
    [[nodiscard]]
    std::vector<eng::obj::PolygonVertexOnly> applyTransformation()const noexcept;
    void draw() override;
    int handle(int event) override;

    [[nodiscard]] static eng::mtr::Matrix
    createNewTransformationMatrix(int width, int height) noexcept;

private:
    RGBArray screenArray;
    RGB backgroundColor, color;
    std::vector<eng::obj::PolygonVertexOnly> polygons;

    void changeModel(eng::mtr::Matrix matrix);
};