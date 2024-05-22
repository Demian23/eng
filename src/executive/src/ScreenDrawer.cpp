#include "ScreenDrawer.h"
#include "../../pipeline/src/Shaders.h"
#include <FL/Fl.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/fl_draw.H>
#include <cstdint>
#include <iostream>

enum class ScreenDrawer::Focused { Target, Camera };
enum class ScreenDrawer::DrawStyle {
    Mesh,
    Phong,
    DeferredPhong,
    Texture,
    FullSpecularWithTextureAlbedo,
    DeferredPhongWithTextures
};

ScreenDrawer::ScreenDrawer(int width, int height, eng::ent::Model &&model,
                           eng::ent::Camera camera,
                           eng::ent::CameraProjection cameraProjection,
                           eng::ent::LightArray&& lights)
    : Fl_Window{width, height}, _model(std::move(model)), _camera(camera),
      _projection(cameraProjection), _pipe{_model, _camera, _projection},
      _lights{std::move(lights)},
      _screenArray{static_cast<uint64_t>(width * height)},
      _indexBuffer(static_cast<uint64_t>(width * height), static_cast<uint32_t>(w())),
      currentFocus{Focused::Target}, currentStyle{DrawStyle::Mesh}
{
    _pipe.setZBufferSize(static_cast<uint32_t>(width * height),
                         static_cast<uint32_t>(w()));
}

void ScreenDrawer::draw()
{
    using eng::ent::PixelArray;
    using namespace eng;
    using namespace eng::shader;

    _screenArray.fill({0, 0, 0});
    _indexBuffer.clean();
    eng::ent::PixelArray::pixel color = currentFocus == Focused::Camera
                                            ? PixelArray::pixel{0, 0, 255}
                                            : PixelArray::pixel{0, 255, 0};
    auto verticesInViewportSpace =
        _pipe.applyVertexTransformations(0, w(), 0, h());
    auto viewportVerticesIt = verticesInViewportSpace.cbegin();
    auto indexColorInserter = [=, this](uint64_t  index, vec::Vec3F color){
      PixelArray::pixel value = {static_cast<uint8_t>(color[0]),
                                 static_cast<uint8_t>(color[1]),
                                 static_cast<uint8_t>(color[2])};
      _screenArray.trySetPixel(index, value);
    };
    auto colorInserter = [=, this, xSize = static_cast<uint32_t>(w())](
                             long long x, long long y,
                             eng::vec::Vec3F targetColor) {
        uint64_t index =
            static_cast<uint32_t>(y) * xSize + static_cast<uint32_t>(x);
        PixelArray::pixel value = {static_cast<uint8_t>(targetColor[0]),
                                   static_cast<uint8_t>(targetColor[1]),
                                   static_cast<uint8_t>(targetColor[2])};
        _screenArray.trySetPixel(index, value);
    };
    auto outGenerator = [=](auto&& valueProducer){
        return [=](uint32_t x, uint32_t y, [[maybe_unused]] floating u,
                  [[maybe_unused]] floating v, [[maybe_unused]] floating w,
                  Triangle triangle){colorInserter(x, y, valueProducer(u, v, w, triangle));};
    };
    auto constantColorInserter =
        [inserter = colorInserter,
         colorAsFloat = eng::vec::Vec3F{static_cast<eng::floating>(color[0]),
                                        static_cast<eng::floating>(color[1]),
                                        static_cast<eng::floating>(color[2])}](
            long long x, long long y) { inserter(x, y, colorAsFloat); };
    auto albedo = _model.getAlbedo();
    auto eye = _camera.getEye();
    auto shine = _model.getShinePower();
    auto textureCoordsIt = _model.textureCoordsBegin();
    auto modelMatrix = _model.getModelMatrix();
    std::vector<Vertex> verticesInWorldSpace{_model.verticesBegin(),
                                             _model.verticesEnd()};
    std::transform(
        verticesInWorldSpace.begin(), verticesInWorldSpace.end(),
        verticesInWorldSpace.begin(),
        [modelMatrix = _model.getModelMatrix()](eng::vec::Vec4F vertex) {
            return modelMatrix * vertex;
        });
    std::vector<Normal> normalsInWorldSpace{_model.normalsBegin(),
                                            _model.normalsEnd()};
    std::transform(
        normalsInWorldSpace.begin(), normalsInWorldSpace.end(),
        normalsInWorldSpace.begin(),
        [modelMatrix = _model.getModelMatrix()](eng::vec::Vec3F normal) {
            return (modelMatrix *
                    vec::Vec4F{normal[0], normal[1], normal[2], 0})
                .trim<3>();
        });

    switch (currentStyle) {
    case DrawStyle::Mesh:
        _pipe.drawMesh(0, w(), 0, h(), constantColorInserter);
        break;
    case DrawStyle::Phong: {
        if (normalsInWorldSpace.empty())
            break;
        NoTexturePhongShader shader{_lights,
                           verticesInWorldSpace.cbegin(),
                           normalsInWorldSpace.cbegin(),
                           albedo,
                           eye,
                           shine};
        _pipe.rasterize(viewportVerticesIt, outGenerator(shader));
        break;
    }
    case DrawStyle::DeferredPhong:
        if(!normalsInWorldSpace.empty()){
            NoTexturePhongShader shader{_lights,
                                        verticesInWorldSpace.cbegin(),
                                        normalsInWorldSpace.cbegin(),
                                        albedo,
                                        eye,
                                        shine};
            _pipe.rasterize(viewportVerticesIt, std::ref(_indexBuffer));
            std::for_each(_indexBuffer.cBegin(), _indexBuffer.cEnd(), [=, index = static_cast<uint64_t>(0)](auto&& fragment)mutable{
                auto [u, v, w, triangle]  = fragment;
                if(triangle.cbegin()->vertexOffset != PolygonComponent::invalidOffset){
                   indexColorInserter(index, shader(u, v, w, triangle));
                }
                index++;
            });
        }
        break;
    case DrawStyle::Texture:
        if (_model.getDiffuseMap() && _model.getNormalMap() &&
            _model.getSpecularMap()) {
            decltype(auto) diffRef = _model.getDiffuseMap();
            decltype(auto) normRef = _model.getNormalMap();
            decltype(auto) specRef = _model.getSpecularMap();
            TextureShader shader{
                _lights,
                TextureAlbedo{viewportVerticesIt, textureCoordsIt,
                              diffRef->data()[0],
                              static_cast<uint32_t>(diffRef->w()),
                              static_cast<uint32_t>(diffRef->h()),
                              static_cast<uint8_t>(diffRef->d())},
                TextureNormal{modelMatrix, viewportVerticesIt, textureCoordsIt,
                              normRef->data()[0],
                              static_cast<uint32_t>(normRef->w()),
                              static_cast<uint32_t>(normRef->h()),
                              static_cast<uint8_t>(normRef->d())},
                TextureSpecular{viewportVerticesIt, textureCoordsIt,
                                specRef->data()[0],
                                static_cast<uint32_t>(specRef->w()),
                                static_cast<uint32_t>(specRef->h()),
                                static_cast<uint8_t>(specRef->d()),eye,
                verticesInWorldSpace.cbegin(),
                shine}};
            _pipe.rasterize(viewportVerticesIt, outGenerator(shader));
        }
        break;
    case DrawStyle::FullSpecularWithTextureAlbedo:
        if (!normalsInWorldSpace.empty() && _model.getDiffuseMap()) {
            decltype(auto) diffRef = _model.getDiffuseMap();
            FullSpecularWithTextureAlbedo shader{
                _lights,
                TextureAlbedo{viewportVerticesIt, textureCoordsIt,
                              diffRef->data()[0],
                              static_cast<uint32_t>(diffRef->w()),
                              static_cast<uint32_t>(diffRef->h()),
                              static_cast<uint8_t>(diffRef->d())},
                verticesInWorldSpace.cbegin(),
                normalsInWorldSpace.cbegin(),
                eye,
                shine};
            _pipe.rasterize(viewportVerticesIt, outGenerator(shader));
        }
        break;
    case DrawStyle::DeferredPhongWithTextures:
        if (_model.getDiffuseMap() && _model.getNormalMap() &&
            _model.getSpecularMap()) {
            decltype(auto) diffRef = _model.getDiffuseMap();
            decltype(auto) normRef = _model.getNormalMap();
            decltype(auto) specRef = _model.getSpecularMap();
            _pipe.rasterize(viewportVerticesIt, std::ref(_indexBuffer));
            TextureShader shader{
                _lights,
                TextureAlbedo{viewportVerticesIt, textureCoordsIt,
                              diffRef->data()[0],
                              static_cast<uint32_t>(diffRef->w()),
                              static_cast<uint32_t>(diffRef->h()),
                              static_cast<uint8_t>(diffRef->d())},
                TextureNormal{modelMatrix, viewportVerticesIt, textureCoordsIt,
                              normRef->data()[0],
                              static_cast<uint32_t>(normRef->w()),
                              static_cast<uint32_t>(normRef->h()),
                              static_cast<uint8_t>(normRef->d())},
                TextureSpecular{viewportVerticesIt, textureCoordsIt,
                                specRef->data()[0],
                                static_cast<uint32_t>(specRef->w()),
                                static_cast<uint32_t>(specRef->h()),
                                static_cast<uint8_t>(specRef->d()),eye,
                                verticesInWorldSpace.cbegin(),
                                shine}};
            std::for_each(_indexBuffer.cBegin(), _indexBuffer.cEnd(), [=, index = static_cast<uint64_t>(0)](auto&& fragment)mutable{
              auto [u, v, w, triangle]  = fragment;
              if(triangle.cbegin()->vertexOffset != PolygonComponent::invalidOffset){
                  indexColorInserter(index, shader(u, v, w, triangle));
              }
              index++;
            });
        }
    }
    fl_draw_image(_screenArray.data(), 0, 0, w(), h(), 3);
}

int ScreenDrawer::handle(int event)
{
    enum HandledEvents {
        ChangeFocusToCamera = 'c',
        ChangeFocusToTarget = 't',
        ChangeProjectionType = 'p',
        ScaleTarget = 's',
        MoveCameraByDiagonal = 'd',
        ChangeCameraAzimuthal = 'a',
        ChangeCameraPolar = 'w',
        ClearAllModelTransformations = '.',
        RotateX = 'x',
        RotateY = 'y',
        RotateZ = 'z',
        MoveX = 'm',
        MoveY = 'u',
        MoveZ = 'f',
        PerspectiveAngle = 'e',
        DrawingStyle = 'o'
    };
    switch (event) {
    case FL_FOCUS:
    case FL_UNFOCUS:
        return 1;
    case FL_KEYBOARD:
        switch (Fl::event_key()) {
        case DrawingStyle:
            switch (currentStyle) {
            case DrawStyle::Mesh:
                currentStyle = DrawStyle::Phong;
                break;
            case DrawStyle::Phong:
                currentStyle = DrawStyle::DeferredPhong;
                break;
            case DrawStyle::DeferredPhong:
                currentStyle = DrawStyle::Texture;
                break;
            case DrawStyle::Texture:
                currentStyle = DrawStyle::FullSpecularWithTextureAlbedo;
                break;
            case DrawStyle::FullSpecularWithTextureAlbedo:
                currentStyle = DrawStyle::DeferredPhongWithTextures;
                break;
            case DrawStyle::DeferredPhongWithTextures:
                currentStyle = DrawStyle::Mesh;
                break;
            }
            break;
        case ChangeFocusToCamera:
            currentFocus = Focused::Camera;
            break;
        case ChangeFocusToTarget:
            currentFocus = Focused::Target;
            break;
        case ChangeProjectionType:
            _pipe.setProjectionType(
                _pipe.getProjectionType() ==
                        eng::ent::ProjectionType::Perspective
                    ? eng::ent::ProjectionType::Orthographic
                    : eng::ent::ProjectionType::Perspective);
            break;
        case ScaleTarget:
            if (currentFocus == Focused::Target) {
                eng::floating scaleOn = Fl::event_shift() ? 0.25 : 2;
                _model.scale(scaleOn);
                break;
            }
            return Fl_Window::handle(event);
        case MoveCameraByDiagonal:
            if (currentFocus == Focused::Camera) {
                eng::floating diagonalAddition =
                    _camera.getDiagonalLength() * 0.1f;
                diagonalAddition =
                    Fl::event_shift() ? -diagonalAddition : diagonalAddition;
                _camera.moveAlongDiagonal(diagonalAddition);
                break;
            }
            return Fl_Window::handle(event);
        case ChangeCameraAzimuthal:
            if (currentFocus == Focused::Camera) {
                eng::floating azimuthalAddition = Fl::event_shift() ? -5 : 5;
                _camera.changeAzimuthalAngle(azimuthalAddition);
                break;
            }
            return Fl_Window::handle(event);
        case ChangeCameraPolar:
            if (currentFocus == Focused::Camera) {
                eng::floating polarAddition = Fl::event_shift() ? -5 : 5;
                _camera.changePolarAngle(polarAddition);
                break;
            }
            return Fl_Window::handle(event);
        case RotateX:
            if (currentFocus == Focused::Camera) {
                eng::floating degree = Fl::event_shift() ? -5 : 5;
                _camera.rotateX(degree);
            }
            if (currentFocus == Focused::Target) {
                eng::floating degree = Fl::event_shift() ? -5 : 5;
                _model.rotateX(degree);
            }
            break;
        case RotateY:
            if (currentFocus == Focused::Camera) {
                eng::floating degree = Fl::event_shift() ? -5 : 5;
                _camera.rotateY(degree);
            }
            if (currentFocus == Focused::Target) {
                eng::floating degree = Fl::event_shift() ? -5 : 5;
                _model.rotateY(degree);
            }
            break;
        case RotateZ:
            if (currentFocus == Focused::Camera) {
                eng::floating degree = Fl::event_shift() ? -5 : 5;
                _camera.rotateZ(degree);
            }
            if (currentFocus == Focused::Target) {
                eng::floating degree = Fl::event_shift() ? -5 : 5;
                _model.rotateZ(degree);
            }
            break;
        case MoveX: {
            auto value = static_cast<eng::floating>(w()) * 0.05f;
            eng::floating addition = Fl::event_shift() ? -value : value;
            if (currentFocus == Focused::Camera) {
                _camera.moveTarget({addition, 0, 0});
            }
            if (currentFocus == Focused::Target) {
                _model.move({addition, 0, 0});
            }
            break;
        }
        case MoveY: {
            auto value = static_cast<eng::floating>(h()) * 0.05f;
            eng::floating addition = Fl::event_shift() ? -value : value;
            if (currentFocus == Focused::Camera) {
                _camera.moveTarget({0, addition, 0});
            }
            if (currentFocus == Focused::Target) {
                _model.move({0, addition, 0});
            }
            break;
        }
        case MoveZ: {
            auto zComponent = _projection.getZComponent();
            auto value = (zComponent.second - zComponent.first) * 0.05f;
            eng::floating addition = Fl::event_shift() ? -value : value;
            if (currentFocus == Focused::Camera) {
                _camera.moveTarget({0, 0, addition});
            }
            if (currentFocus == Focused::Target) {
                _model.move({0, 0, addition});
            }
            break;
        }
        case PerspectiveAngle:
            if (_pipe.getProjectionType() ==
                eng::ent::ProjectionType::Perspective) {
                eng::floating addition = Fl::event_shift() ? -5 : 5;
                _projection.setAngleInDegrees(_projection.getAngleInDegrees() +
                                              addition);
                break;
            }
            return Fl_Window::handle(event);
        case ClearAllModelTransformations:
            if (currentFocus == Focused::Target) {
                _model.clearModelMatrix();
                break;
            }
            return Fl_Window::handle(event);
        default:
            return Fl_Window::handle(event);
        }
        redraw();
        return 1;
    default:
        return Fl_Window::handle(event);
    }
}
