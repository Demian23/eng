#include <FL/Fl.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Window.H>
#include <cstdlib>
#include <vector>
#include "../../objparser/src/ObjParser.h"
#include <fstream>
#include "../../matrix/src/Scale.h"
#include "../../matrix/src/View.h"
#include "../../matrix/src/Viewport.h"
#include "../../matrix/src/OrthographicProjection.h"

constexpr auto XSize = 1000, YSize = 1000;
class An_Image_Window
    :
    public Fl_Window
{
    std::vector<eng::obj::Vertex> vertices;
public:
    An_Image_Window(int w, int h)
        :
          Fl_Window(w, h)
    {
        std::vector<eng::obj::Normal> normals;
        std::vector<eng::obj::Polygon> polygons;

        std::ifstream file("/Users/egor/Downloads/FinalBaseMesh.obj");
        eng::obj::parse(file, vertices, normals, polygons);
        auto xMin = 0, xMax = xMin + XSize, yMin = 0, yMax = yMin + YSize, zMin = 1, zMax= 100;
        eng::vec::ThreeDimensionalVector eye{100, -100, 100}, target{200, 200, 200}, up{0, -1, 0}, scale{50, 50, 0};
        auto resultTransformations = eng::mtr::Viewport{xMin, xMax, yMin, yMax} *
                                     eng::mtr::OrthographicProjection{xMin, xMax, yMin, yMax, zMin, zMax} *
                                     eng::mtr::View{eye, target, up} * eng::mtr::Scale{scale};
        for(auto& vertex : vertices){
            vertex = resultTransformations * vertex;
        }
    }

    virtual void draw()
    {
        constexpr auto arrsize = XSize * YSize * 3;
        uint8_t data[arrsize];
        memset(data, 255, arrsize);
        for(const auto& vertex : vertices){
            size_t index = (((size_t) vertex[1] * XSize + (size_t)vertex[0])) * 3;
            if(index < arrsize && index > 0) {
                data[index] = 0;
                data[index + 1] = 0;
                data[index + 2] = 0;
            }
        }
        fl_draw_image(data, 0, 0, XSize, YSize, 3);
    }
};
int main(int argc, char *argv[])
{
    An_Image_Window window(XSize, YSize);
    window.end();
    window.show(argc, argv);
    return Fl::run();
}
