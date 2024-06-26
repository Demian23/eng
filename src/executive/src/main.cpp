#include "../../objparser/src/ObjParser.h"
#include "ScreenDrawer.h"
#include <FL/Fl.H>
#include <FL/Fl_BMP_Image.H>
#include <FL/Fl_JPEG_Image.H>
#include <FL/Fl_PNG_Image.H>
#include <algorithm>
#include <cxxopts.hpp>
#include <memory>
#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

std::string readArgsAndReturnPathToObj(
    int argc, const char *argv[], eng::ent::Model &modelForInit,
    eng::ent::Camera &cameraForInit,
    eng::ent::CameraProjection &projectionForInit,
    eng::ent::LightArray &lightsForInit, bool &isAutoPositioning);

void addTextureMaps(eng::ent::Model &model, std::string_view diffuseMapPath,
                    std::string_view normalMapPath,
                    std::string_view specularMapPath);
void autoPositioning(eng::ent::Model &model, eng::ent::Camera &camera,
                     eng::ent::CameraProjection &projection);

void initModel(std::string_view pathToObj, eng::ent::Model &model);
void getDirectionalLights(eng::ent::LightArray& lights, std::vector<eng::floating> lightsParams);
void getPointLights(eng::ent::LightArray& lights, std::vector<eng::floating> lightsParams);

void exceptionHandler();

int main(int argc, const char *argv[])
{
    std::cout.exceptions(std::iostream::badbit | std::iostream::failbit);
    std::cerr.exceptions(std::iostream::badbit | std::iostream::failbit);
    int x, y, w, h;
    Fl::screen_xywh(x, y, w, h);
    try {
        eng::ent::Model model{};
        eng::ent::Camera camera{};
        eng::ent::CameraProjection projection{static_cast<eng::floating>(w),
                                              static_cast<eng::floating>(h)};
        eng::ent::LightArray lights{};
        bool isAutoPositioning{false};

        auto path = readArgsAndReturnPathToObj(
            argc, argv, model, camera, projection, lights, isAutoPositioning);
        initModel(path, model);

        if (isAutoPositioning)
            autoPositioning(model, camera, projection);

        ScreenDrawer drawer{w,      h - 20,     std::move(model),
                            camera, projection, std::move(lights)};
        drawer.end();
        drawer.show();
        return Fl::run();
    } catch (...) {
        exceptionHandler();
    }
    return 1;
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

void autoPositioning(eng::ent::Model &model, eng::ent::Camera &camera,
                     eng::ent::CameraProjection &projection)
{
    auto [xMin, xMax, yMin, yMax, zMin, zMax] =
        eng::alg::boundingBox<3>(model.verticesBegin(), model.verticesEnd());
    auto modelWidth = xMax - xMin;
    auto modelHeight = yMax - yMin;
    auto modelThickness = zMax - zMin;

    model.addModelTransformation(
        eng::mtr::Matrix::getMove({0, -modelHeight / 2, -modelThickness / 2}));

    std::cout << "Height: " << modelHeight << " Width: " << modelWidth << " Thickness: " << modelThickness << std::endl;

    projection.setZComponent(0.01f, std::max(modelThickness * 10, 100.f));

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
    camera.reset({distanceForCamera, 0, 0}, {0, 0, 0}, {0, 1, 0});
}

std::string readArgsAndReturnPathToObj(
    int argc, const char *argv[], eng::ent::Model &modelForInit,
    eng::ent::Camera &cameraForInit,
    eng::ent::CameraProjection &projectionForInit,
    eng::ent::LightArray &lightsForInit, bool &isAutoPositioning)
{

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
        "scale", "Scale model in all directions on coefficients vector (1,1,1)",
        cxxopts::value<std::vector<eng::floating>>()->default_value("1,1,1"))(
        "f,fov", "Fov angle in projection matrix",
        cxxopts::value<eng::floating>())(
        "albedo", "Albedo as vector with 3 float in range 0 to 1",
        cxxopts::value<std::vector<eng::floating>>()->default_value(
            "0.18,0.18,0.18"))("diffuse-map", "Path to diffuse map for model",
                               cxxopts::value<std::string>())(
        "normal-map", "Path to normal map for model",
        cxxopts::value<std::string>())("specular-map",
                                       "Path to normal map for model",
                                       cxxopts::value<std::string>())(
        "shine-power", "Shine power coefficient in Phong",
        cxxopts::value<eng::floating>())
        ("directional-lights","Array of lights in notation: light-direction vector, light color, light intensity. Example: 0,0,1.0,255,255,255,0.8,1,0,0,1,1,1,0.2 -> two light sources", cxxopts::value<std::vector<eng::floating>>())
        ("point-lights","Array of lights in notation: light position vector, light color, light intensity, light attenuation params (constant, linear, exp). Example: 0,0,1.0,255,255,255,0.8,1,0.2,0.7 -> one light sources", cxxopts::value<std::vector<eng::floating>>());

    options.parse_positional({"source"});

    auto result = options.parse(argc, argv);
    if (result.count("help")) {
        throw std::logic_error{options.help()};
    }

    std::string source{};
    if (result.count("source")) {
        source = result["source"].as<std::string>();
    } else {
        throw std::logic_error("No obj file!");
    }

    auto vecToVec3F = [](const std::vector<eng::floating> &vec) {
        return eng::vec::Vec3F{vec.at(0), vec.at(1), vec.at(2)};
    };
    if(result.count("directional-lights")) {
        getDirectionalLights(
            lightsForInit,
            result["directional-lights"].as<std::vector<eng::floating>>());
    }
    if(result.count("point-lights")){
        getPointLights(lightsForInit, result["point-lights"].as<std::vector<eng::floating>>());
    }
    modelForInit.setAlbedo(
        vecToVec3F(result["albedo"].as<std::vector<eng::floating>>()));
    if (result.count("fov"))
        projectionForInit.setAngleInDegrees(result["fov"].as<eng::floating>());
    std::string diffuseMap{}, normalMap{}, specularMap{};
    if (result.count("diffuse-map")) {
        diffuseMap = result["diffuse-map"].as<std::string>();
    }
    if (result.count("normal-map")) {
        normalMap = result["normal-map"].as<std::string>();
    }
    if (result.count("specular-map")) {
        specularMap = result["specular-map"].as<std::string>();
    }
    if (result.count("shine-power")) {
        modelForInit.setShinePower(result["shine-power"].as<eng::floating>());
    }
    addTextureMaps(modelForInit, diffuseMap, normalMap, specularMap);

    isAutoPositioning = result["auto-positioning"].as<bool>();
    if (!isAutoPositioning) {
        auto cameraEye{
            vecToVec3F(result["camera"].as<std::vector<eng::floating>>())};
        cameraEye[1] = eng::degreeToRadian(cameraEye[1]);
        cameraEye[2] = eng::degreeToRadian(cameraEye[2]);
        auto targetPosition{
            vecToVec3F(result["target"].as<std::vector<eng::floating>>())};
        cameraForInit.reset(cameraEye, targetPosition, {0, 1, 0});
        modelForInit.addModelTransformation(eng::mtr::Matrix::getScale(
            vecToVec3F(result["scale"].as<std::vector<eng::floating>>())));
    }
    return source;
}

void initModel(std::string_view pathToObj, eng::ent::Model &model)
{
    std::ifstream objFile{};

    if (!std::filesystem::is_regular_file(pathToObj)) {
        throw std::logic_error(std::string(pathToObj) + " is not regular file");
    }

    objFile.open(pathToObj.data());
    if (objFile.bad()) {
        throw std::logic_error{"Can't open " + std::string(pathToObj) +
                               " for read"};
    }
    std::vector<eng::Vertex> vertices;
    std::vector<eng::Normal> normals;
    std::vector<eng::TextureCoord> textures;
    std::vector<eng::Triangle> triangles;

    eng::obj::parseStream(objFile, vertices, normals, textures, triangles);
    model.reset(std::move(vertices), std::move(triangles), std::move(normals),
                std::move(textures));
}

std::unique_ptr<Fl_RGB_Image> getRGBImage(std::string_view pathToImage);

void addTextureMaps(eng::ent::Model &model, std::string_view diffuseMapPath,
                    std::string_view normalMapPath,
                    std::string_view specularMapPath)
{
    if (!diffuseMapPath.empty()) {
        model.setDiffuseMap(getRGBImage(diffuseMapPath));
    }
    if (!normalMapPath.empty()) {
        model.setNormalMap(getRGBImage(normalMapPath));
    }
    if (!specularMapPath.empty()) {
        model.setSpecularMap(getRGBImage(specularMapPath));
    }
}

std::unique_ptr<Fl_RGB_Image> getRGBImage(std::string_view pathToImage)
{
    using eng::obj::operator""sv;
    using std::filesystem::path;

    if (std::filesystem::is_regular_file(pathToImage)) {
        std::array allowedTextureMapExtensions = {".jpg"sv, ".png"sv, ".bmp"sv};
        enum extensions { JPEG = 0, PNG = 1, BMP = 2 };
        auto iter = std::find(allowedTextureMapExtensions.begin(),
                              allowedTextureMapExtensions.end(),
                              path{pathToImage}.extension());
        if (iter != allowedTextureMapExtensions.end()) {
            auto specificExtension = static_cast<extensions>(
                iter - allowedTextureMapExtensions.begin());
            switch (specificExtension) {
            case JPEG:
                return std::make_unique<Fl_JPEG_Image>(pathToImage.data());
            case PNG:
                return std::make_unique<Fl_PNG_Image>(pathToImage.data());
            case BMP:
                return std::make_unique<Fl_BMP_Image>(pathToImage.data());
            }
        } else {
            throw std::logic_error{"Only JPEG, PNG, BMP extensions are "
                                   "supported for texture maps"};
        }
    }
    return nullptr;
}

void getDirectionalLights(eng::ent::LightArray& lights, std::vector<eng::floating> lightsParams){
    auto size = lightsParams.size();
    if((size % 7) != 0){
        throw std::logic_error{"Wrong directional lights params count"};
    }
    for(unsigned i = 0; i < size; i+=7 ){
        auto direction = eng::vec::Vec3F {lightsParams[i], lightsParams[i+1], lightsParams[i+2]};
        auto color = eng::vec::Vec3F {lightsParams[i+3], lightsParams[i+4], lightsParams[i+5]};
        auto intensity = lightsParams[i+6];
        lights.push_back(std::make_unique<eng::ent::DistantLight>(direction, color, intensity));
    }
}

void getPointLights(eng::ent::LightArray& lights, std::vector<eng::floating> lightsParams){
    auto size = lightsParams.size();
    if((size % 10) != 0){
        throw std::logic_error{"Wrong directional lights params count"};
    }
    for(unsigned i = 0; i < size; i+=10 ){
        auto position = eng::vec::Vec3F {lightsParams[i], lightsParams[i+1], lightsParams[i+2]};
        auto color = eng::vec::Vec3F {lightsParams[i+3], lightsParams[i+4], lightsParams[i+5]};
        auto intensity = lightsParams[i+6];
        auto attenuationConstant = lightsParams[i+7];
        auto attenuationLinear = lightsParams[i+8];
        auto attenuationExp = lightsParams[i+9];
        lights.push_back(std::make_unique<eng::ent::PointLight>(position, color, intensity, attenuationConstant, attenuationLinear, attenuationExp));
    }
}
