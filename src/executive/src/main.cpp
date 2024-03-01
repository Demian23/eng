#include "../../objparser/src/ObjParser.h"
#include "MonoColoredScreenDrawer.h"
#include <cxxopts.hpp>
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
                        std::pair<eng::floating, eng::floating> z,
                        eng::mtr::Matrix modelTransformation);

eng::mtr::Matrix getModelTransformationFromUserString(const char *argv[]);

void exceptionHandler();

int main(int argc, const char *argv[])
{
    std::cout.exceptions(std::iostream::badbit | std::iostream::failbit);
    std::cerr.exceptions(std::iostream::badbit | std::iostream::failbit);

    cxxopts::Options options("Executive", "Executive system, illustrating eng library work");

    // ?
    options.add_options()
        ("h,help", "Print usage")
        ("a,auto-positioning", "Auto positioning of model")
        ("source", ".obj with model", cxxopts::value<std::string>())
        ("c,camera", "Camera position vector in polar coordinates", cxxopts::value<std::vector<eng::floating>>()->default_value("3,0,0"))
        ("t,target", "Target position vector", cxxopts::value<std::vector<eng::floating>>()->default_value("0,0,0"))
        ("scale", "Scale model in all directions on one coefficient", cxxopts::value<eng::floating>())
    ;
    options.parse_positional({"source"});

    try{
        auto result = options.parse(argc, argv);
        if(result.count("help")){
            std::cout << options.help() << std::endl;
            return 0;
        }
        std::string source{};
        if(result.count("source")){
            source = result["source"].as<std::string>();
        }else{
            std::cerr << "No obj file!\n";
            return 1;
        }
        auto camera = result["camera"].as<std::vector<eng::floating>>();
        auto target = result["target"].as<std::vector<eng::floating>>();
        eng::vec::ThreeDimensionalVector cameraEye {camera.at(0), eng::degreeToRadian(camera.at(1)), eng::degreeToRadian(camera.at(2))},
            targetPosition{target.at(0), target.at(1), target.at(2)};
        eng::mtr::Matrix modelTransformation =
            eng::mtr::Matrix::createIdentityMatrix();
        if(result.count("scale")){
            auto scaleCoefficient = result["scale"].as<eng::floating>();
            modelTransformation = modelTransformation * eng::mtr::Scale{{scaleCoefficient, scaleCoefficient, scaleCoefficient}};
        }
        return initExecutiveAndRun(source, cameraEye, targetPosition,
                                   {0.0f, 100.0f}, modelTransformation);
    } catch (...) {
        exceptionHandler();
    }

    return 1;
}

struct Dimensions{eng::floating xMin, xMax, yMin, yMax, zMin, zMax;};

using vertexConsIter = std::vector<eng::Vertex>::const_iterator;

Dimensions findVerticesDimensions(vertexConsIter begin, vertexConsIter end)
{
    Dimensions dimensions{};
    auto vertexWithMinimalX = std::min_element(begin, end,
                                               [](auto&& firstVertex, auto&& secondVertex){return *firstVertex.begin() < *secondVertex.begin();});
    dimensions.xMin = (*vertexWithMinimalX)[0];
    auto vertexWithMinimalY = std::min_element(begin, end,
                                               [](auto&& firstVertex, auto&& secondVertex){return *(firstVertex.begin() + 1) < *(secondVertex.begin() + 1);});
    dimensions.yMin = (*vertexWithMinimalY)[1];
    auto vertexWithMinimalZ = std::min_element(begin, end,
                                               [](auto&& firstVertex, auto&& secondVertex){return *(firstVertex.begin() + 2) < *(secondVertex.begin() + 2);});
    dimensions.zMin = (*vertexWithMinimalZ)[2];

    auto vertexWithMaximalX = std::max_element(begin, end,
                                               [](auto&& firstVertex, auto&& secondVertex){return *firstVertex.begin() < *secondVertex.begin();});
    dimensions.xMax = (*vertexWithMaximalX)[0];
    auto vertexWithMaximalY = std::max_element(begin, end,
                                               [](auto&& firstVertex, auto&& secondVertex){return *(firstVertex.begin() + 1) < *(secondVertex.begin() + 1);});
    dimensions.yMax = (*vertexWithMaximalY)[1];
    auto vertexWithMaximalZ = std::max_element(begin, end,
                                               [](auto&& firstVertex, auto&& secondVertex){return *(firstVertex.begin() + 2) < *(secondVertex.begin() + 2);});
    dimensions.zMax = (*vertexWithMaximalZ)[2];
    return dimensions;
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

    auto createDrawerAndRun = [=, &objFile]<PolygonType T>(std::vector<T> polygons){
      std::vector<eng::Vertex> vertices;
      eng::obj::parseOnlyVerticesAndPolygons(objFile, vertices, polygons);
      auto dimensions = findVerticesDimensions(vertices.begin(), vertices.end());
      eng::ent::Model model{std::move(polygons)};
      model.addModelTransformation(modelTransformation);

      MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera,
                                     projection);
      drawer.end();
      drawer.show();
      return Fl::run();
    };

    switch (numberOfVertices) {
    case 3:
        return createDrawerAndRun(std::vector<TriangleVertexOnly>());
    case 4:
        return createDrawerAndRun(std::vector<QuadVertexOnly>());
    default:
        return createDrawerAndRun(std::vector<PolygonVertexOnly>());
    }
}

void exceptionHandler()
{
    try {
        throw;
    } catch (const cxxopts::exceptions::exception& ex){
        std::cerr << ex.what() << '\n';
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    } catch (...) {
        std::cerr << "Unknown exception threw\n";
    }
}

eng::mtr::Matrix getModelTransformationFromUserString(const char *argv[])
{
    enum transformations {
        scale = 's',
        rotateX = 'x',
        rotateY = 'y',
        rotateZ = 'z',
    };
    eng::mtr::Matrix result = eng::mtr::Matrix::createIdentityMatrix();
    for (; *argv != nullptr; ++argv) {
        switch (**argv) {
        case scale: {
            auto scaleValue = std::stof(*(++argv));
            result =
                eng::mtr::Scale{{scaleValue, scaleValue, scaleValue}} * result;
            break;
        }
        case rotateX: {
            auto degree = std::stof(*(++argv));
            result = eng::mtr::RotateX{degree} * result;
            break;
        }
        case rotateY: {
            auto degree = std::stof(*(++argv));
            result = eng::mtr::RotateY{degree} * result;
            break;
        }
        case rotateZ: {
            auto degree = std::stof(*(++argv));
            result = eng::mtr::RotateZ{degree} * result;
            break;
        }
        default:
            break;
        }
    }
    return result;
}
