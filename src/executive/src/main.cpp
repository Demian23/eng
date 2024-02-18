#include "../../objparser/src/ObjParser.h"
#include "MonoColoredScreenDrawer.h"
#include <FL/Fl.H>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <stdexcept>

int initExecutiveAndRun(std::string_view pathToObjFile,
                        eng::vec::ThreeDimensionalVector cameraEye, 
                        eng::vec::ThreeDimensionalVector target);
void exceptionHandler();


int main(int argc, const char *argv[])
{
    std::cout.exceptions(std::iostream::badbit | std::iostream::failbit);
    std::cerr.exceptions(std::iostream::badbit | std::iostream::failbit);

    try{
        if(argc != 5 && argc != 8){
            std::cerr << "usage: " << argv[0]
                      << " <path-to-obj-file> < camera position: "
                         "<radialLine> <polarAngle> <azimuthalAngle> > [target position: <x> <y> <z>]";
        }else{
            eng::vec::ThreeDimensionalVector cameraEye{
                std::stof(argv[2]),
                eng::degreeToRadian(std::stof(argv[3])),
                eng::degreeToRadian(std::stof(argv[4]))
            };
            eng::vec::ThreeDimensionalVector target{};
            if(argc == 8){
                target = {std::stof(argv[5]), std::stof(argv[6]), std::stof(argv[7])};
            }
            return initExecutiveAndRun(argv[1], cameraEye, target);
        }
    } catch (...) {
        exceptionHandler();
    }
    return 1;
}


template <typename PolygonType>
std::vector<PolygonType> getPolygons(std::istream& objStream)
{
    std::vector<eng::Vertex> vertices;
    std::vector<PolygonType> polygons;
    eng::obj::parseOnlyVerticesAndPolygons(objStream, vertices, polygons);
    return polygons;
}


int initExecutiveAndRun(std::string_view pathToObjFile,
                        eng::vec::ThreeDimensionalVector cameraEye,
                        eng::vec::ThreeDimensionalVector target)
{
    using namespace eng;
    using namespace eng::obj;
    using namespace eng::vec;
    using namespace eng::ent;

    if(!std::filesystem::is_regular_file(pathToObjFile)){
        throw std::logic_error(std::string(pathToObjFile) + " is not regular file");
    }

    std::ifstream objFile(pathToObjFile);
    if(objFile.bad()) {
        throw std::logic_error{"Can't open " + std::string(pathToObjFile) +
                             " for read"};
    }
    
    uint32_t numberOfVertices = checkPolygonSize(objFile);
    objFile.seekg(0);
    int x, y, w, h;
    Fl::screen_xywh(x, y, w, h);
    
    ThreeDimensionalVector defaultUp{0, 1, 0};
    Camera camera{cameraEye, target, defaultUp};
    CameraProjection projection{static_cast<floating>(w), static_cast<floating>(h), 0.1f, 2, 90};

    switch (numberOfVertices) {
    case 3: {
        eng::ent::Model model{getPolygons<TriangleVertexOnly>(objFile)};
        MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera, projection);
        drawer.end();
        drawer.show();
        return Fl::run();
    }
    case 4: {
        eng::ent::Model model{getPolygons<QuadVertexOnly>(objFile)};
        MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera, projection);
        drawer.end();
        drawer.show();
        return Fl::run();
    }
    default:
        eng::ent::Model model{getPolygons<PolygonVertexOnly>(objFile)};
        MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera, projection);
        drawer.end();
        drawer.show();
        return Fl::run();
    }
}

void exceptionHandler()
{
    try{
        throw;    
    } catch(const std::exception& e){
        std::cerr << e.what() << '\n';
    } catch(...){
        std::cerr << "Unknown exception threw\n";
    }
}
