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


class MonoColorDrawer final : public Fl_Window{
public:
    MonoColorDrawer(int xSize, int ySize);
    inline void setBackgroundColor(RGB newColor){backgroundColor = newColor;}
    inline void setColor(RGB newColor){color = newColor;}
    void setNewPolygons(std::vector<eng::obj::PolygonVertexOnly>&& newPolygons);
    eng::mtr::Matrix createNewTransformationMatrix(eng::mtr::Matrix additionalMatrix);
    std::vector<eng::obj::PolygonVertexOnly> applyCurrentTransformation();
    void draw() override;
    int handle(int event) override;

private:
    eng::mtr::Matrix currentTransformation;
    RGBArray screenArray;
    RGB backgroundColor, color;
    std::vector<eng::obj::PolygonVertexOnly> polygons;

    void changeModel(eng::mtr::Matrix matrix);
};