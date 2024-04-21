#include "ScreenDrawer.h"
#include "../../pipeline/src/Shaders.h"

enum class ScreenDrawer::Focused { Target, Camera };
enum class ScreenDrawer::DrawStyle {
    Mesh,
    LambertRule,
    PhongShadingWithLambert,
    PhongShadingWithPhong
};

ScreenDrawer::ScreenDrawer(int width, int height, eng::ent::Model &&model,
                           eng::ent::Camera camera,
                           eng::ent::CameraProjection cameraProjection)
    : Fl_Window{width, height}, _model(std::move(model)), _camera(camera),
      _projection(cameraProjection), _pipe{_model, _camera, _projection},
      _light{}, screenArray{static_cast<uint64_t>(width * height)},
      currentFocus{Focused::Target}, currentStyle{DrawStyle::Mesh}
{
    _pipe.setZBufferSize(static_cast<uint32_t>(width * height),
                         static_cast<uint32_t>(w()));
    _light.color = {200, 60, 200};
}

void ScreenDrawer::draw()
{
    screenArray.fill({0, 0, 0});
    RGB color =
        currentFocus == Focused::Camera ? RGB{0, 0, 255} : RGB{0, 255, 0};
    auto colorInserter =
        [=, this, xSize = static_cast<uint32_t>(w())](long long x, long long y,
                                                      eng::vec::Vec3F color) {
            screenArray[static_cast<uint32_t>(y) * xSize +
                        static_cast<uint32_t>(x)] = {
                static_cast<uint8_t>(color[0]), static_cast<uint8_t>(color[1]),
                static_cast<uint8_t>(color[2])};
        };
    auto constantColorInserter = [inserter = colorInserter, colorAsFloat = eng::vec::Vec3F{static_cast<eng::floating>(color.r), static_cast<eng::floating>(color.g), static_cast<eng::floating>(color.b)}](long long x, long long y){inserter(x, y, colorAsFloat);};
    switch (currentStyle) {
    case DrawStyle::Mesh:
        _pipe.drawMesh(0, w(), 0, h(), constantColorInserter);
        break;
    case DrawStyle::LambertRule: {
        eng::pipe::FlatShaderWithLambertColoring shader{
            _model.getModelMatrix(), _light, colorInserter,
            _model.verticesBegin()};
        _pipe.rasterize(0, w(), 0, h(), shader);
        break;
    }
    case DrawStyle::PhongShadingWithLambert: {
        std::vector<eng::Normal> normalsCopy{_model.normalsBegin(),
                                             _model.normalsEnd()};
        if(normalsCopy.empty())
            break;
        std::transform(
            normalsCopy.begin(), normalsCopy.end(), normalsCopy.begin(),
            [modelMatrix = _model.getModelMatrix()](eng::vec::Vec3F normal) {
                return (modelMatrix *
                        eng::vec::Vec4F{normal[0], normal[1], normal[2], 0})
                    .trim<3>();
            });
        eng::pipe::PhongShaderWithLambertColoring shader{
            _light, colorInserter, normalsCopy.begin()};
        _pipe.rasterize(0, w(), 0, h(), shader);
        break;
    }
    case DrawStyle::PhongShadingWithPhong: {
        std::vector<eng::Normal> normalsCopy{_model.normalsBegin(),
                                             _model.normalsEnd()};
        std::vector<eng::Vertex> verticesCopy{_model.verticesBegin(),
                                             _model.verticesEnd()};
        if(normalsCopy.empty())
            break;
        std::transform(
            normalsCopy.begin(), normalsCopy.end(), normalsCopy.begin(),
            [modelMatrix = _model.getModelMatrix()](eng::vec::Vec3F normal) {
              return (modelMatrix *
                      eng::vec::Vec4F{normal[0], normal[1], normal[2], 0})
                  .trim<3>();
            });
        std::transform(
            verticesCopy.begin(), verticesCopy.end(), verticesCopy.begin(),
            [modelMatrix = _model.getModelMatrix()](eng::vec::Vec4F vertex) {
              return modelMatrix * vertex;
            });
        eng::pipe:: PhongShaderWithPhongColoring shader{
            _light, colorInserter, verticesCopy.cbegin(), normalsCopy.cbegin(), _model.getAlbedo(), 128, _camera.getEye()};
        _pipe.rasterize(0, w(), 0, h(), shader);
        break;
    }
    }
    fl_draw_image(screenArray.data(), 0, 0, w(), h(), 3);
}

void ScreenDrawer::printOutStatistic(std::ostream &stream)
{
    auto [cameraX, cameraY, cameraZ] =
        static_cast<std::array<eng::floating, 3>>(_camera.getEye());
    auto [targetX, targetY, targetZ] =
        static_cast<std::array<eng::floating, 3>>(_camera.getTarget());
    auto [diagonal, azimuth, polar] = static_cast<std::array<eng::floating, 3>>(
        eng::vec::cartesianToSpherical(_camera.getEye()));
    stream << "Camera: " << cameraX << ' ' << cameraY << ' ' << cameraZ << '\n'
           << "Target: " << targetX << ' ' << targetY << ' ' << targetZ << '\n'
           << "Camera (diagonal azimuth polar): " << diagonal << ' '
           << eng::radianToDegree(azimuth) << ' ' << eng::radianToDegree(polar)
           << '\n';
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
        Statistic = 'i',
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
                currentStyle = DrawStyle::LambertRule;
                break;
            case DrawStyle::LambertRule:
                currentStyle = DrawStyle::PhongShadingWithLambert;
                break;
            case DrawStyle::PhongShadingWithLambert:
                currentStyle = DrawStyle::PhongShadingWithPhong;
                break;
            case DrawStyle::PhongShadingWithPhong:
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
            auto value = static_cast<eng::floating>(zComponent.second -
                                                    zComponent.first) *
                         0.05f;
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
        case Statistic:
            printOutStatistic(std::cout);
            return 1;
        default:
            return Fl_Window::handle(event);
        }
        redraw();
        return 1;
    default:
        return Fl_Window::handle(event);
    }
}
