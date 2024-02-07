#pragma once

#include "../../matrix/src/Move.h"
#include "../../matrix/src/Rotate.h"
#include "../../matrix/src/Scale.h"
#include "../../matrix/src/View.h"
#include "../../matrix/src/Viewport.h"
#include "../../matrix/src/OrthographicProjection.h"
#include "../../objparser/src/PolygonVertexOnly.h"
#include "RGBArray.h"

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>


class MonoColorDrawer final : public Fl_Window{
public:
    MonoColorDrawer(int xSize, int ySize);
    inline void setBackgroundColor(RGB newColor){backgroundColor = newColor;}
    inline void setColor(RGB newColor){color = newColor;}
    void setNewPolygons(std::vector<eng::obj::PolygonVertexOnly>&& newPolygons);
    void draw() override;
private:
    RGBArray screenArray;
    eng::mtr::Matrix currentTransformation;
    RGB backgroundColor, color;
    std::vector<eng::obj::PolygonVertexOnly> polygons;
};