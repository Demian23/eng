#pragma once

#include "../../algorithm/src/alg.h"
#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"
#include "../../entities/src/Model.h"
#include "../../matrix/src/Viewport.h"
#include "RGBArray.h"
#include "RGBArrayInserter.h"
#include <format>
#include <ostream>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/fl_draw.H>
#include <algorithm>

template <typename PolygonType>
class MonoColoredScreenDrawer final : public Fl_Window {
public:
    MonoColoredScreenDrawer(int width, int height,
                            eng::ent::Model<PolygonType> &&aModel,
                            eng::ent::Camera aCamera,
                            eng::ent::CameraProjection aCameraProjection)
        : Fl_Window{width, height}, model{std::move(aModel)}, camera{aCamera},
          cameraProjection{aCameraProjection},
          screenArray{static_cast<uint64_t>(width * height)}
    {}

    [[nodiscard]] std::vector<PolygonType> applyTransformations() const noexcept
    {
        using namespace eng;
        using namespace eng::mtr;
        using namespace eng::vec;
        using namespace std;

        std::vector<PolygonType> copy{model.getPolygonsBegin(),
                                      model.getPolygonsEnd()};

        auto transformationMatrix =
            Viewport{0, 0 + static_cast<floating>(w()), 0,
                     0 + static_cast<floating>(h())} *
            cameraProjection.getProjectionMatrix(projectionType) *
            camera.getViewMatrix() * model.getModelMatrix();
        auto transformVertex = [transformationMatrix,
                                projection =
                                    this->projectionType](auto &&vertex) {
            vertex = transformationMatrix * vertex;
            if (projection == eng::ent::ProjectionType::Perspective) {
                transform(vertex.begin(), vertex.end(), vertex.begin(),
                          [w = *vertex.rbegin()](auto &&coord) {
                              return w > 1 ? coord /= w : 0;
                          });
            }
            return vertex;
        };
        for (auto &polygon : copy) {
            transform(polygon.begin(), polygon.end(), polygon.begin(),
                      transformVertex);
        }
        return copy;
    }

    void draw() override
    {
        screenArray.fill(backgroundColor);
        if (!model.empty()) {
            auto copy = applyTransformations();
            RGBArrayInserter inserter{screenArray, color,
                                      static_cast<uint64_t>(this->w())};
            for (const auto &polygon : copy) {
                eng::alg::line(
                    static_cast<eng::numeric>(polygon[0][0]),
                    static_cast<eng::numeric>((*polygon.rbegin())[0]),
                    static_cast<eng::numeric>(polygon[0][1]),
                    static_cast<eng::numeric>((*polygon.rbegin())[1]),
                    inserter);
                for (unsigned i = 1; i < polygon.size(); i++) {
                    eng::alg::line(static_cast<eng::numeric>(polygon[i - 1][0]),
                                   static_cast<eng::numeric>(polygon[i][0]),
                                   static_cast<eng::numeric>(polygon[i - 1][1]),
                                   static_cast<eng::numeric>(polygon[i][1]),
                                   inserter);
                }
            }
        }
        fl_draw_image(screenArray.data(), 0, 0, w(), h(), 3);
    }

    void printOutStatistic(std::ostream& stream)
    {
        auto [cameraX, cameraY, cameraZ] = static_cast<std::array<eng::floating, 3>>(camera.getEye());
        auto [targetX, targetY, targetZ] = static_cast<std::array<eng::floating, 3>>(camera.getTarget());
        auto [diagonal, azimuth, polar] = static_cast<std::array<eng::floating, 3>>(eng::vec::cartesianToSpherical(camera.getEye()));
        stream << "Camera: " << cameraX << ' ' << cameraY << ' ' << cameraZ << '\n'
            << "Target: " << targetX << ' ' << targetY << ' ' << targetZ << '\n'
            << "Camera (diagonal azimuth polar): " << diagonal << ' ' << eng::radianToDegree(azimuth) << ' ' << eng::radianToDegree(polar) << '\n';
    }

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

    enum class Focused { Camera, Target };

    static constexpr RGB cameraFocusedColor = {255, 0, 0};
    static constexpr RGB targetFocusedColor = {0, 0, 255};

    int handle(int event) override
    {
        switch (event) {
        case FL_FOCUS:
        case FL_UNFOCUS:
            return 1;
        case FL_KEYBOARD:
            switch (Fl::event_key()) {
            case ChangeFocusToCamera:
                color = cameraFocusedColor;
                currentFocus = Focused::Camera;
                break;
            case ChangeFocusToTarget:
                color = targetFocusedColor;
                currentFocus = Focused::Target;
                break;
            case ChangeProjectionType:
                projectionType =
                    projectionType == eng::ent::ProjectionType::Perspective
                        ? eng::ent::ProjectionType::Orthographic
                        : eng::ent::ProjectionType::Perspective;
                break;
            case ScaleTarget:
                if (currentFocus == Focused::Target) {
                    eng::floating scaleOn = Fl::event_shift() ? 0.25 : 2;
                    model.scale(scaleOn);
                    break;
                }
                return Fl_Window::handle(event);
            case MoveCameraByDiagonal:
                if (currentFocus == Focused::Camera) {
                    eng::floating diagonalAddition =
                        camera.getDiagonalLength() * 0.1f;
                    diagonalAddition = Fl::event_shift() ? -diagonalAddition
                                                         : diagonalAddition;
                    camera.moveAlongDiagonal(diagonalAddition);
                    break;
                }
                return Fl_Window::handle(event);
            case ChangeCameraAzimuthal:
                if (currentFocus == Focused::Camera) {
                    eng::floating azimuthalAddition =
                        Fl::event_shift() ? -5 : 5;
                    camera.changeAzimuthalAngle(azimuthalAddition);
                    break;
                }
                return Fl_Window::handle(event);
            case ChangeCameraPolar:
                if (currentFocus == Focused::Camera) {
                    eng::floating polarAddition = Fl::event_shift() ? -5 : 5;
                    camera.changePolarAngle(polarAddition);
                    break;
                }
                return Fl_Window::handle(event);
            case RotateX:
                if (currentFocus == Focused::Camera) {
                    eng::floating degree = Fl::event_shift() ? -5 : 5;
                    camera.rotateX(degree);
                }
                if (currentFocus == Focused::Target) {
                    eng::floating degree = Fl::event_shift() ? -5 : 5;
                    model.rotateX(degree);
                }
                break;
            case RotateY:
                if (currentFocus == Focused::Camera) {
                    eng::floating degree = Fl::event_shift() ? -5 : 5;
                    camera.rotateY(degree);
                }
                if (currentFocus == Focused::Target) {
                    eng::floating degree = Fl::event_shift() ? -5 : 5;
                    model.rotateY(degree);
                }
                break;
            case RotateZ:
                if (currentFocus == Focused::Camera) {
                    eng::floating degree = Fl::event_shift() ? -5 : 5;
                    camera.rotateZ(degree);
                }
                if (currentFocus == Focused::Target) {
                    eng::floating degree = Fl::event_shift() ? -5 : 5;
                    model.rotateZ(degree);
                }
                break;
            case MoveX: {
                auto value = static_cast<eng::floating>(w()) * 0.05f;
                eng::floating addition = Fl::event_shift() ? -value : value;
                if (currentFocus == Focused::Camera) {
                    camera.moveTarget({addition, 0, 0});
                }
                if (currentFocus == Focused::Target) {
                    model.move({addition, 0, 0});
                }
                break;
            }
            case MoveY: {
                auto value = static_cast<eng::floating>(h()) * 0.05f;
                eng::floating addition = Fl::event_shift() ? -value : value;
                if (currentFocus == Focused::Camera) {
                    camera.moveTarget({0, addition, 0});
                }
                if (currentFocus == Focused::Target) {
                    model.move({0, addition, 0});
                }
                break;
            }
            case MoveZ: {
                auto zComponent = cameraProjection.getZComponent();
                auto value = static_cast<eng::floating>(zComponent.second -
                                                        zComponent.first) *
                             0.05f;
                eng::floating addition = Fl::event_shift() ? -value : value;
                if (currentFocus == Focused::Camera) {
                    camera.moveTarget({0, 0, addition});
                }
                if (currentFocus == Focused::Target) {
                    model.move({0, 0, addition});
                }
                break;
            }
            case PerspectiveAngle:
                if (projectionType == eng::ent::ProjectionType::Perspective) {
                    eng::floating addition = Fl::event_shift() ? -5 : 5;
                    cameraProjection.setAngleInDegrees(
                        cameraProjection.getAngleInDegrees() + addition);
                    break;
                }
                return Fl_Window::handle(event);
            case ClearAllModelTransformations:
                if (currentFocus == Focused::Target) {
                    model.clearModelMatrix();
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

    inline void setBackgroundColor(RGB newColor) { backgroundColor = newColor; }

private:
    eng::ent::Model<PolygonType> model;
    eng::ent::Camera camera;
    eng::ent::CameraProjection cameraProjection;
    RGBArray screenArray;
    RGB backgroundColor{0xFF, 0xFF, 0xFF}, color{targetFocusedColor};
    eng::ent::ProjectionType projectionType{
        eng::ent::ProjectionType::Orthographic};
    Focused currentFocus{Focused::Target};
};
