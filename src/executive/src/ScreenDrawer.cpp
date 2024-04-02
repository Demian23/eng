#include "ScreenDrawer.h"

enum class ScreenDrawer::Focused { Target, Camera };

ScreenDrawer::ScreenDrawer(int width, int height, eng::ent::Model &&model,
                           eng::ent::Camera camera,
                           eng::ent::CameraProjection cameraProjection)
    : Fl_Window{width, height}, _model(std::move(model)), _camera(camera),
      _projection(cameraProjection), _pipe{_model, _camera, _projection},
      screenArray{static_cast<uint64_t>(width * height)},
      currentFocus{Focused::Target}
{}

void ScreenDrawer::draw()
{
    screenArray.fill({0, 0, 0});
    auto copy = _pipe.applyVertexTransformations(0, w(), 0, h());
    auto copyIterator = copy.cbegin();
    RGB color =
        currentFocus == Focused::Camera ? RGB{0, 0, 255} : RGB{0, 255, 0};
    std::for_each(
        _pipe.trianglesBegin(), _pipe.trianglesEnd(),
        [=, inserter = RGBArrayInserter{screenArray, color,
                                        static_cast<uint32_t>(w())}](
            auto &&polygon) mutable {
            eng::alg::line(static_cast<eng::integral>(
                               (*(copyIterator + polygon[0].vertexOffset))[0]),
                           static_cast<eng::integral>(
                               (*(polygon[2].vertexOffset + copyIterator))[0]),
                           static_cast<eng::integral>(
                               (*(copyIterator + polygon[0].vertexOffset))[1]),
                           static_cast<eng::integral>(
                               (*(polygon[2].vertexOffset + copyIterator))[1]),
                           inserter);
            eng::alg::line(static_cast<eng::integral>(
                               (*(copyIterator + polygon[0].vertexOffset))[0]),
                           static_cast<eng::integral>(
                               (*(polygon[1].vertexOffset + copyIterator))[0]),
                           static_cast<eng::integral>(
                               (*(copyIterator + polygon[0].vertexOffset))[1]),
                           static_cast<eng::integral>(
                               (*(polygon[1].vertexOffset + copyIterator))[1]),
                           inserter);
            eng::alg::line(static_cast<eng::integral>(
                               (*(copyIterator + polygon[1].vertexOffset))[0]),
                           static_cast<eng::integral>(
                               (*(polygon[2].vertexOffset + copyIterator))[0]),
                           static_cast<eng::integral>(
                               (*(copyIterator + polygon[1].vertexOffset))[1]),
                           static_cast<eng::integral>(
                               (*(polygon[2].vertexOffset + copyIterator))[1]),
                           inserter);
        });
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
    };
    switch (event) {
    case FL_FOCUS:
    case FL_UNFOCUS:
        return 1;
    case FL_KEYBOARD:
        switch (Fl::event_key()) {
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
