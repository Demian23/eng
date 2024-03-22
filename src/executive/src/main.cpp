#include "../../objparser/src/ObjParser.h"
#include "MonoColoredScreenDrawer.h"
#include <FL/Fl.H>
#include <cxxopts.hpp>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <istream>
#include <stdexcept>

int initExecutiveAndRun(std::string_view pathToObjFile, eng::ent::Camera camera,
                        eng::mtr::Matrix modelTransformation,
                        bool isAutoPositioning);

void exceptionHandler();
struct Dimensions {
    eng::floating xMin, xMax, yMin, yMax, zMin, zMax;
};
using vertexConsIter = std::vector<eng::Vertex>::const_iterator;
Dimensions findModelDimensions(vertexConsIter begin, vertexConsIter end);
uint32_t
openObjFileAndGetNumberOfVerticesInPolygon(std::string_view pathToObjFile,
                                           std::ifstream &objFile);

void autoPositioningForModel(Dimensions dimensions, eng::ent::Camera &camera,
                             eng::ent::CameraProjection &projection,
                             eng::mtr::Matrix &modelTransformation);

int main(int argc, const char *argv[])
{
    std::cout.exceptions(std::iostream::badbit | std::iostream::failbit);
    std::cerr.exceptions(std::iostream::badbit | std::iostream::failbit);

    cxxopts::Options options("Executive",
                             "Executive system, illustrating eng library work");

    options.add_options()("h,help", "Print usage")(
        "a,auto-positioning", "Auto positioning of model",
        cxxopts::value<bool>()->default_value("false"))(
        "source", ".obj with model", cxxopts::value<std::string>())(
        "c,camera", "Camera position vector in polar coordinates",
        cxxopts::value<std::vector<eng::floating>>()->default_value("3,0,0"))(
        "t,target", "Target position vector",
        cxxopts::value<std::vector<eng::floating>>()->default_value("0,0,0"))(
        "scale", "Scale model in all directions on one coefficient",
        cxxopts::value<eng::floating>());
    options.parse_positional({"source"});

    try {
        auto result = options.parse(argc, argv);
        if (result.count("help")) {
            std::cout << options.help() << std::endl;
            return 0;
        }
        std::string source{};
        if (result.count("source")) {
            source = result["source"].as<std::string>();
        } else {
            std::cerr << "No obj file!\n";
            return 1;
        }

        eng::ent::Camera camera{};

        eng::mtr::Matrix modelTransformation =
            eng::mtr::Matrix::createIdentityMatrix();

        // TODO: get vectors here and then make auto-positioning
        // and pass model, camera and projection to drawer
        // if use auto-positioning and set some values you know what you doing?
        bool isAutoPositioning = result["auto-positioning"].as<bool>();
        if (!isAutoPositioning) {
            auto cameraVec = result["camera"].as<std::vector<eng::floating>>();
            auto target = result["target"].as<std::vector<eng::floating>>();
            eng::vec::ThreeDimensionalVector cameraEye{
                cameraVec.at(0), eng::degreeToRadian(cameraVec.at(1)),
                eng::degreeToRadian(cameraVec.at(2))},
                targetPosition{target.at(0), target.at(1), target.at(2)};
            camera.reset(cameraEye, targetPosition, {0, 1, 0});
            if (result.count("scale")) {
                auto scaleCoefficient = result["scale"].as<eng::floating>();
                modelTransformation =
                    modelTransformation *
                    eng::mtr::Scale{
                        {scaleCoefficient, scaleCoefficient, scaleCoefficient}};
            }
        }

        return initExecutiveAndRun(source, camera, modelTransformation,
                                   isAutoPositioning);
    } catch (...) {
        exceptionHandler();
    }

    return 1;
}

int initExecutiveAndRun(std::string_view pathToObjFile, eng::ent::Camera camera,
                        eng::mtr::Matrix modelTransformation,
                        bool isAutoPositioning)

{
    std::ifstream objFile{};
    auto numberOfVerticesInPolygon =
        openObjFileAndGetNumberOfVerticesInPolygon(pathToObjFile, objFile);

    auto createDrawerAndRun = [=, &objFile]<eng::ent::PolygonType T>(
                                  std::vector<T> polygons) mutable {
        std::vector<eng::Vertex> vertices;

        eng::obj::parseOnlyVerticesAndPolygons(objFile, vertices, polygons);
        int x, y, w, h;
        Fl::screen_xywh(x, y, w, h);
        eng::ent::CameraProjection projection{static_cast<eng::floating>(w),
                                              static_cast<eng::floating>(h), 0,
                                              100, 90};

        if (isAutoPositioning) {
            autoPositioningForModel(
                findModelDimensions(vertices.begin(), vertices.end()), camera,
                projection, modelTransformation);
        }

        eng::ent::Model model{std::move(polygons)};
        model.addModelTransformation(modelTransformation);
        MonoColoredScreenDrawer drawer(w, h - 20, std::move(model), camera,
                                       projection);
        drawer.end();
        drawer.show();
        return Fl::run();
    };

    switch (numberOfVerticesInPolygon) {
    case 3:
        return createDrawerAndRun(std::vector<eng::TriangleVertexOnly>{});
    case 4:
        return createDrawerAndRun(std::vector<eng::QuadVertexOnly>{});
    default:
        return createDrawerAndRun(std::vector<eng::PolygonVertexOnly>{});
    }
}

void exceptionHandler()
{
    try {
        throw;
    } catch (const cxxopts::exceptions::exception &ex) {
        std::cerr << ex.what() << '\n';
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
    } catch (...) {
        std::cerr << "Unknown exception threw\n";
    }
}
uint32_t
openObjFileAndGetNumberOfVerticesInPolygon(std::string_view pathToObjFile,
                                           std::ifstream &objFile)
{
    if (!std::filesystem::is_regular_file(pathToObjFile)) {
        throw std::logic_error(std::string(pathToObjFile) +
                               " is not regular file");
    }

    objFile.open(pathToObjFile);
    if (objFile.bad()) {
        throw std::logic_error{"Can't open " + std::string(pathToObjFile) +
                               " for read"};
    }

    auto result = eng::obj::getPolygonSize(objFile);
    objFile.seekg(0);
    return result;
}

Dimensions findModelDimensions(vertexConsIter begin, vertexConsIter end)
{
    Dimensions dimensions{};
    auto comparatorGenerator = [](int addition) {
        return [addition](auto &&firstVertex, auto &&secondVertex) {
            return *(firstVertex.begin() + addition) <
                   *(secondVertex.begin() + addition);
        };
    };
    auto compareX = comparatorGenerator(0);
    auto compareY = comparatorGenerator(1);
    auto compareZ = comparatorGenerator(2);

    dimensions.xMin = (*std::min_element(begin, end, compareX))[0];
    dimensions.yMin = (*std::min_element(begin, end, compareY))[1];
    dimensions.zMin = (*std::min_element(begin, end, compareZ))[2];
    dimensions.xMax = (*std::max_element(begin, end, compareX))[0];
    dimensions.yMax = (*std::max_element(begin, end, compareY))[1];
    dimensions.zMax = (*std::max_element(begin, end, compareZ))[2];

    return dimensions;
}

void autoPositioningForModel(Dimensions dimensions, eng::ent::Camera &camera,
                             eng::ent::CameraProjection &projection,
                             eng::mtr::Matrix &modelTransformation)
{
    auto modelWidth = dimensions.xMax - dimensions.xMin;
    auto modelHeight = dimensions.yMax - dimensions.yMin;
    auto modelThickness = dimensions.zMax - dimensions.zMin;

    modelTransformation =
        modelTransformation *
        eng::mtr::Move{{0, -modelHeight / 2, -modelThickness / 2}};
    projection.setZComponent(dimensions.zMin + 1, dimensions.zMax + 1);

    eng::floating diagonalForCircumscribedParallelepiped =
        std::sqrt(modelWidth * modelWidth + modelHeight * modelHeight +
                  modelThickness * modelThickness);
    eng::floating radiusForSphere = diagonalForCircumscribedParallelepiped / 2;

    eng::floating fovDivided = eng::degreeToRadian(projection.getAngleInDegrees() / 2);
    eng::floating horizontalFovDivided = std::atan(projection.getAspect() * std::tan(fovDivided));

    eng::floating distanceForCamera = std::max({radiusForSphere / std::sin(std::min(fovDivided, horizontalFovDivided)), projection.getZMin() + radiusForSphere});

    // TODO: ask how fix this
    camera.reset({distanceForCamera, 0, 0}, {0, 0, 0}, {0, 1, 0});
}
