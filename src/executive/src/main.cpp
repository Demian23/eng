#include <FL/Fl.H>
#include "MonoColorDrawer.h"
#include "../../objparser/src/ObjParser.h"
#include <filesystem>
#include <iostream>
#include <fstream>

std::vector<eng::obj::PolygonVertexOnly> getPolygons(std::string_view pathToObjFile){
    std::vector<eng::obj::PolygonVertexOnly> polygons;
    if(std::filesystem::is_regular_file(pathToObjFile)){
        std::fstream objFile(pathToObjFile);
        std::vector<eng::obj::Vertex> vertices;
        eng::obj::parseOnlyVerticesAndPolygons(objFile, vertices, polygons);
    }else{
        std::cerr << pathToObjFile << "is not a regular file!\n";
    }
    return polygons;
}

int main(int argc, char *argv[])
{
    if(argc == 2){
        MonoColorDrawer drawer(1000, 1000);
        drawer.end();
        drawer.setBackgroundColor({100, 0, 0});
        drawer.setColor({0, 100, 0});
        drawer.setNewPolygons(getPolygons(argv[1]));
        drawer.show();
        return Fl::run();
    }else{
        std::cerr << "usage: " << argv[0] << " <path-to-obj-file>";
        return 1;
    }
}
