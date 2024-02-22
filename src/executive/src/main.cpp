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
                        eng::vec::ThreeDimensionalVector target,
                        std::pair<eng::floating, eng::floating> z, eng::mtr::Matrix modelTransformation);

eng::mtr::Matrix getModelTransformationFromUserString(const char*argv[]);

void exceptionHandler();

int main(int argc, const char *argv[])
{
    std::cout.exceptions(std::iostream::badbit | std::iostream::failbit);
    std::cerr.exceptions(std::iostream::badbit | std::iostream::failbit);

    try {
        if (argc != 5 && argc != 8 && (argc < 10)) {
            std::cerr << "usage: " << argv[0]
                      << " <path-to-obj-file> < camera position: "
                         "<radialLine> <polarAngle> <azimuthalAngle> > [target "
                         "position: <x> <y> <z>] [zNear zFar] [model_transformations]\nModel transformation example: s 0.1 x 30 y 10 z 0 m 1,1,1\n";
        } else {
            eng::vec::ThreeDimensionalVector cameraEye{
                std::stof(argv[2]), eng::degreeToRadian(std::stof(argv[3])),
                eng::degreeToRadian(std::stof(argv[4]))};
            eng::vec::ThreeDimensionalVector target{};
            eng::floating zNear = 0.1f, zFar = 1000;
            eng::mtr::Matrix modelTransformation = eng::mtr::Matrix::createIdentityMatrix();
            if (argc >= 8) {
                target = {std::stof(argv[5]), std::stof(argv[6]),
                          std::stof(argv[7])};
            }
            if (argc >= 10) {
                zNear = std::stof(argv[8]);
                zFar = std::stof(argv[9]);
            }
            if(argc > 10){
                modelTransformation = getModelTransformationFromUserString(argv + 10);
            }
            return initExecutiveAndRun(argv[1], cameraEye, target,
                                       {zNear, zFar}, modelTransformation);
        }
    } catch (...) {
        exceptionHandler();
    }
    return 1;
}

template <typename PolygonType>
std::vector<PolygonType> getPolygons(std::istream &objStream)
{
    std::vector<eng::Vertex> vertices;
    std::vector<PolygonType> polygons;
    eng::obj::parseOnlyVerticesAndPolygons(objStream, vertices, polygons);
    return polygons;
}

int initExecutiveAndRun(std::string_view pathToObjFile,
                        eng::vec::ThreeDimensionalVector cameraEye,
                        eng::vec::ThreeDimensionalVector target,
                        std::pair<eng::floating, eng::floating> z,
                        eng::mtr::Matrix modelTransformation)
{
    using namespace eng;
    using namespace eng::obj;
    using namespace eng::vec;
    using namespace eng::ent;

    if (!std::filesystem::is_regular_file(pathToObjFile)) {
        throw std::logic_error(std::string(pathToObjFile) +
                               " is not regular file");
    }

    std::ifstream objFile(pathToObjFile);
    if (objFile.bad()) {
        throw std::logic_error{"Can't open " + std::string(pathToObjFile) +
                               " for read"};
    }

    uint32_t numberOfVertices = checkPolygonSize(objFile);
    objFile.seekg(0);
    int x, y, w, h;
    Fl::screen_xywh(x, y, w, h);

    ThreeDimensionalVector defaultUp{0, 1, 0};
    Camera camera{cameraEye, target, defaultUp};
    CameraProjection projection{static_cast<floating>(w),
                                static_cast<floating>(h), z.first, z.second,
                                90};

    switch (numberOfVertices) {
    case 3: {
        eng::ent::Model model{getPolygons<TriangleVertexOnly>(objFile)};
        model.addModelTransformation(modelTransformation);
        MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera,
                                       projection);
        drawer.end();
        drawer.show();
        return Fl::run();
    }
    case 4: {
        eng::ent::Model model{getPolygons<QuadVertexOnly>(objFile)};
        model.addModelTransformation(modelTransformation);
        MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera,
                                       projection);
        drawer.end();
        drawer.show();
        return Fl::run();
    }
    default:
        eng::ent::Model model{getPolygons<PolygonVertexOnly>(objFile)};
        model.addModelTransformation(modelTransformation);
        MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera,
                                       projection);
        drawer.end();
        drawer.show();
        return Fl::run();
    }
}

void exceptionHandler()
{
    try {
        throw;
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    } catch (...) {
        std::cerr << "Unknown exception threw\n";
    }
}

eng::mtr::Matrix getModelTransformationFromUserString(const char*argv[])
{
    enum transformations{scale = 's', rotateX = 'x', rotateY = 'y', rotateZ = 'z', move = 'm'};
    eng::mtr::Matrix result = eng::mtr::Matrix::createIdentityMatrix();
    for(;*argv != nullptr; ++argv){
        switch (**argv) {
        case scale:{
            auto scaleValue = std::stof(*(++argv));
            result = eng::mtr::Scale{{scaleValue, scaleValue, scaleValue}} * result;
            break;
        }
        case rotateX:{
            auto degree = std::stof(*(++argv));
            result = eng::mtr::RotateX{degree} * result;
            break;
        }
        case rotateY:{
            auto degree = std::stof(*(++argv));
            result = eng::mtr::RotateY{degree} * result;
            break;
        }
        case rotateZ:{
            auto degree = std::stof(*(++argv));
            result = eng::mtr::RotateZ{degree} * result;
            break;
        }
        case move:
        default:
            break;
        }
    }
    return result;
}
