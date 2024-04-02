#include "../../objparser/src/ObjParser.h"
#include "ScreenDrawer.h"
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

using vertexConstIter = std::vector<eng::Vertex>::const_iterator;

uint32_t
openObjFileAndGetNumberOfVerticesInPolygon(std::string_view pathToObjFile,
                                           std::ifstream &objFile);

void autoPositioningForModel(std::array<eng::floating, 6> dimensions,
                             eng::ent::Camera &camera,
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

        bool isAutoPositioning = result["auto-positioning"].as<bool>();
        if (!isAutoPositioning) {
            auto cameraVec = result["camera"].as<std::vector<eng::floating>>();
            auto target = result["target"].as<std::vector<eng::floating>>();
            eng::vec::Vec3F cameraEye{cameraVec.at(0),
                                      eng::degreeToRadian(cameraVec.at(1)),
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

    if (!std::filesystem::is_regular_file(pathToObjFile)) {
        throw std::logic_error(std::string(pathToObjFile) +
                               " is not regular file");
    }

    objFile.open(pathToObjFile);
    if (objFile.bad()) {
        throw std::logic_error{"Can't open " + std::string(pathToObjFile) +
                               " for read"};
    }
    std::vector<eng::Vertex> vertices;
    std::vector<eng::Normal> normals;
    std::vector<eng::TextureCoord> textures;
    std::vector<eng::Triangle> triangles;

    eng::obj::parseStream(objFile, vertices, normals, textures, triangles);
    int x, y, w, h;
    Fl::screen_xywh(x, y, w, h);
    eng::ent::CameraProjection projection{static_cast<eng::floating>(w),
                                          static_cast<eng::floating>(h), 0, 100,
                                          90};
    if (isAutoPositioning) {
        autoPositioningForModel(eng::alg::boundingBox<vertexConstIter, 3>(
                                    vertices.begin(), vertices.end()),
                                camera, projection, modelTransformation);
    }
    eng::ent::Model model{std::move(vertices), std::move(triangles),
                          std::move(normals), std::move(textures)};
    model.addModelTransformation(modelTransformation);
    ScreenDrawer drawer{w, h - 20, std::move(model), camera, projection};
    drawer.end();
    drawer.show();
    return Fl::run();
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

void autoPositioningForModel(std::array<eng::floating, 6> dimensions,
                             eng::ent::Camera &camera,
                             eng::ent::CameraProjection &projection,
                             eng::mtr::Matrix &modelTransformation)
{
    auto [xMin, xMax, yMin, yMax, zMin, zMax] = dimensions;
    auto modelWidth = xMax - xMin;
    auto modelHeight = yMax - yMin;
    auto modelThickness = zMax - zMin;

    modelTransformation =
        modelTransformation *
        eng::mtr::Move{{0, -modelHeight / 2, -modelThickness / 2}};
    projection.setZComponent(zMin + 1, zMax + 1); // TODO: here z buffer problem

    eng::floating diagonalForCircumscribedParallelepiped =
        std::sqrt(modelWidth * modelWidth + modelHeight * modelHeight +
                  modelThickness * modelThickness);
    eng::floating radiusForSphere = diagonalForCircumscribedParallelepiped / 2;

    eng::floating fovDivided =
        eng::degreeToRadian(projection.getAngleInDegrees() / 2);
    eng::floating horizontalFovDivided =
        std::atan(projection.getAspect() * std::tan(fovDivided));

    eng::floating distanceForCamera = std::max(
        {radiusForSphere / std::sin(std::min(fovDivided, horizontalFovDivided)),
         projection.getZMin() + radiusForSphere});

    // TODO: ask how fix this
    camera.reset({distanceForCamera, 0, 0}, {0, 0, 0}, {0, 1, 0});
}
