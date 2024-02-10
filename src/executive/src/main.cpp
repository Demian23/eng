#include "../../objparser/src/ObjParser.h"
#include "MonoColorDrawer.h"
#include <FL/Fl.H>
#include <filesystem>
#include <fstream>
#include <iostream>

std::vector<eng::obj::PolygonVertexOnly>
getPolygons(std::string_view pathToObjFile)
{
    std::vector<eng::obj::PolygonVertexOnly> polygons;
    if (std::filesystem::is_regular_file(pathToObjFile)) {
        std::fstream objFile(pathToObjFile);
        std::vector<eng::obj::Vertex> vertices;
        eng::obj::parseOnlyVerticesAndPolygons(objFile, vertices, polygons);
    } else {
        std::cerr << pathToObjFile << "is not a regular file!\n";
    }
    return polygons;
}

int main(int argc, const char *argv[])
{
    if (argc == 2) {
        int x, y, w, h;
        Fl::screen_xywh(x, y, w, h);
        MonoColorDrawer drawer(w, h - 20);
        drawer.end();
        drawer.setBackgroundColor({0xFF, 0xFF, 0xFF});
        drawer.setColor({0, 0, 100});
        drawer.setNewPolygons(getPolygons(argv[1]));
        drawer.show();
        return Fl::run();
    } else {
        std::cerr << "usage: " << argv[0] << " <path-to-obj-file>";
        return 1;
    }
}
