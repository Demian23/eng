#include "../../objparser/src/ObjParser.h"
#include "MonoColorDrawer.h"
#include <FL/Fl.H>
#include <filesystem>
#include <fstream>
#include <iostream>

template <typename PolygonType>
std::vector<PolygonType>
getPolygons(std::string_view pathToObjFile)
{
    std::fstream objFile(pathToObjFile);
    std::vector<eng::obj::Vertex> vertices;
    std::vector<PolygonType> polygons;
    eng::obj::parseOnlyVerticesAndPolygons(objFile, vertices, polygons);
    return polygons;
}

int initExecutiveAndRun(std::string_view pathToObjFile){
    if (std::filesystem::is_regular_file(pathToObjFile)) {
        uint32_t numberOfVertices = 0;
        {
            std::fstream objFile(pathToObjFile);
            if(objFile.good()){
                numberOfVertices = eng::obj::checkPolygonSize(objFile);
            }else{
                std::cerr << pathToObjFile << " mistake happens\n";
                abort();
            }
        }
        int x, y, w, h;
        Fl::screen_xywh(x, y, w, h);
        switch(numberOfVertices){
        case 3: {
            MonoColorDrawer<eng::obj::TriangleVertexOnly> drawer(w, h - 20);
            drawer.end();
            drawer.setBackgroundColor({0xFF, 0xFF, 0xFF});
            drawer.setColor({0, 0, 100});
            drawer.setNewPolygons(getPolygons<eng::obj::TriangleVertexOnly>(pathToObjFile));
            drawer.show();
            return Fl::run();
        }
        case 4: {
            MonoColorDrawer<eng::obj::QuadVertexOnly> drawer(w, h - 20);
            drawer.end();
            drawer.setBackgroundColor({0xFF, 0xFF, 0xFF});
            drawer.setColor({0, 0, 100});
            drawer.setNewPolygons(
                getPolygons<eng::obj::QuadVertexOnly>(pathToObjFile));
            drawer.show();
            return Fl::run();
        }
        default:
            MonoColorDrawer<eng::obj::PolygonVertexOnly> drawer(w, h - 20);
            drawer.end();
            drawer.setBackgroundColor({0xFF, 0xFF, 0xFF});
            drawer.setColor({0, 0, 100});
            drawer.setNewPolygons(
                getPolygons<eng::obj::PolygonVertexOnly>(pathToObjFile));
            drawer.show();
            return Fl::run();
        }
    } else {
        std::cerr << pathToObjFile << "is not a regular file!\n";
        abort();
    }

}

int main(int argc, const char *argv[])
{
    if (argc == 2) {
        return initExecutiveAndRun(argv[1]);
    } else {
        std::cerr << "usage: " << argv[0] << " <path-to-obj-file>";
        return 1;
    }
}
