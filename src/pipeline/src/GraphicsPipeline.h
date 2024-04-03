#pragma once

#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"
#include "../../entities/src/Light.h"
#include "../../entities/src/Model.h"
#include "../../rasterization/src/rast.h"
#include <numbers>

namespace eng::pipe {

class GraphicsPipeline final {
public:
    GraphicsPipeline(ent::Model &model, ent::Camera &camera,
                     ent::CameraProjection &projection);

    [[nodiscard]] std::vector<Vertex>
    applyVertexTransformations(int minX, int maxX, int minY,
                               int maxY) const noexcept;

    [[nodiscard]] std::vector<Normal>
    applyNormalTransformations() const noexcept;

    [[nodiscard]] ent::ProjectionType getProjectionType() const noexcept;
    void setProjectionType(ent::ProjectionType newProjectionType) noexcept;

    void setZBufferSize(uint32_t bufferSize, uint32_t xSize);

    template <typename Out>
    void drawMesh(int minX, int maxX, int minY, int maxY, Out out)
    {
        auto verticesCopy = applyVertexTransformations(minX, maxX, minY, maxY);
        auto copyIterator = verticesCopy.cbegin();
        std::for_each(
            _model.trianglesBegin(), _model.trianglesEnd(),
            [=, cameraEye = _camera.getEye()](auto &&polygon) mutable {
                auto a = (copyIterator + polygon[0].vertexOffset)
                             ->template trim<3>();
                auto b = (copyIterator + polygon[1].vertexOffset)
                             ->template trim<3>();
                auto c = (copyIterator + polygon[2].vertexOffset)
                             ->template trim<3>();

                auto triangleNormal = vec::cross(c - a, b - a);
                auto eyeDirection = a - cameraEye;
                auto normalProduct = eyeDirection * triangleNormal;

                if (normalProduct >= 0) {
                    alg::drawTriangle(a.template convert<2, integral>(),
                                      b.template convert<2, integral>(),
                                      c.template convert<2, integral>(), out);
                }
            });
    }

    template <typename Out>
    void rasterWithLambertRule(int minX, int maxX, int minY, int maxY, Out out)
    {
        auto verticesCopy = applyVertexTransformations(minX, maxX, minY, maxY);
        auto normalsCopy = applyNormalTransformations();
        std::fill(_zBuffer.begin(), _zBuffer.end(), 1);
        auto vertices = verticesCopy.cbegin();
        std::for_each(
            _model.trianglesBegin(), _model.trianglesEnd(),
            [=, l = light, cameraEye = _camera.getEye(),
             zBufferIter = _zBuffer.begin(), zBufferSize = _zBuffer.size(),
             xSize = _xSize,
             albedo = _model.getAlbedo()](auto &&polygon) mutable {
                auto a =
                    (vertices + polygon[0].vertexOffset)->template trim<3>();
                auto b =
                    (vertices + polygon[1].vertexOffset)->template trim<3>();
                auto c =
                    (vertices + polygon[2].vertexOffset)->template trim<3>();

                auto triangleNormal = vec::cross(c - a, b - a);
                auto eyeDirection = a - cameraEye;
                auto normalProduct = eyeDirection * triangleNormal;

                if (normalProduct >= 0) {
                    auto zBufferChecker =
                        [=, azInverse = 1 / static_cast<long double>((a[2])),
                         bzInverse = 1 / static_cast<long double>(b[2]),
                         czInverse = 1 / static_cast<long double>(c[2]),
                         aNormal =
                             *(normalsCopy.begin() + polygon[0].normalOffset),
                         bNormal =
                             *(normalsCopy.begin() + polygon[1].normalOffset),
                         cNormal =
                             *(normalsCopy.begin() + polygon[2].normalOffset)](
                            uint32_t x, uint32_t y, floating u, floating v,
                            floating w) mutable {
                            long double z =
                                1 / (azInverse * static_cast<long double>(u) +
                                     bzInverse * static_cast<long double>(v) +
                                     czInverse * static_cast<long double>(w));
                            auto point = zBufferIter + y * xSize + x;
                            if (y * xSize + x < zBufferSize && z < *point) {

                                Normal normal =
                                    Normal{
                                        aNormal[0] * u + bNormal[0] * v +
                                            cNormal[0] * w,
                                        aNormal[1] * v + bNormal[1] * v +
                                            cNormal[1] * v,
                                        aNormal[2] * w + bNormal[2] * w +
                                            cNormal[2] * w,
                                    }
                                        .normalize();
                                auto product = normal * l.direction.trim<3>();
                                product = std::max(0.f, product);
                                auto resultColor = albedo;
                                resultColor = {resultColor[0] * l.color[0] *
                                                   product * l.intensity,
                                               resultColor[1] * l.color[1] *
                                                   product * l.intensity,
                                               resultColor[2] * l.color[2] *
                                                   product * l.intensity};
                                out(x, y, resultColor);
                                *point = z;
                            }
                        };

                    eng::rast::
                        generateBarycentricCoordinatesForEachPointInTriangle(
                            {a[0], a[1]}, {b[0], b[1]}, {c[0], c[1]},
                            zBufferChecker);
                }
            });
    }

    // with zBuffer check and back-face culling
    template <typename Out>
    void facingRatio(int minX, int maxX, int minY, int maxY, Out out)
    {
        auto verticesCopy = applyVertexTransformations(minX, maxX, minY, maxY);
        auto normals = _model.normalsBegin();
        std::fill(_zBuffer.begin(), _zBuffer.end(), 1);
        auto vertices = verticesCopy.cbegin();
        std::for_each(
            _model.trianglesBegin(), _model.trianglesEnd(),
            [=, l = light, cameraEye = _camera.getEye(),
             zBufferIter = _zBuffer.begin(), zBufferSize = _zBuffer.size(),
             xSize = _xSize,
             albedo = _model.getAlbedo()](auto &&polygon) mutable {
                auto a =
                    (vertices + polygon[0].vertexOffset)->template trim<3>();
                auto b =
                    (vertices + polygon[1].vertexOffset)->template trim<3>();
                auto c =
                    (vertices + polygon[2].vertexOffset)->template trim<3>();

                auto triangleNormal = vec::cross(c - a, b - a);
                auto eyeDirection = a - cameraEye;
                auto normalProduct = eyeDirection * triangleNormal;

                if (normalProduct >= 0) {
                    auto zBufferChecker =
                        [=, azInverse = 1 / static_cast<long double>((a[2])),
                         bzInverse = 1 / static_cast<long double>(b[2]),
                         czInverse = 1 / static_cast<long double>(c[2]),
                         aNormal = *(normals + polygon[0].normalOffset),
                         bNormal = *(normals + polygon[1].normalOffset),
                         cNormal = *(normals + polygon[2].normalOffset)](
                            uint32_t x, uint32_t y, floating u, floating v,
                            floating w) mutable {
                            long double z =
                                1 / (azInverse * static_cast<long double>(u) +
                                     bzInverse * static_cast<long double>(v) +
                                     czInverse * static_cast<long double>(w));
                            auto point = zBufferIter + y * xSize + x;
                            if (y * xSize + x < zBufferSize && z < *point) {
                                Normal normal =
                                    Normal{
                                        aNormal[0] * u + bNormal[0] * v +
                                            cNormal[0] * w,
                                        aNormal[1] * v + bNormal[1] * v +
                                            cNormal[1] * v,
                                        aNormal[2] * w + bNormal[2] * w +
                                            cNormal[2] * w,
                                    }
                                        .normalize();
                                auto pointToEye =
                                    (vec::Vec3F{static_cast<floating>(x),
                                                static_cast<floating>(y),
                                                static_cast<floating>(z)} -
                                     cameraEye)
                                        .normalize();
                                auto product = (normal * pointToEye);
                                floating facingRatio = std::max(0.f, product);
                                out(x, y, facingRatio);
                                *point = z;
                            }
                        };

                    eng::rast::
                        generateBarycentricCoordinatesForEachPointInTriangle(
                            {a[0], a[1]}, {b[0], b[1]}, {c[0], c[1]},
                            zBufferChecker);
                }
            });
    }

private:
    ent::Model &_model;
    ent::Camera &_camera;
    ent::CameraProjection &_projection;
    std::vector<long double> _zBuffer;
    ent::DistantLight light;
    uint32_t _xSize;
    ent::ProjectionType _projectionType;
};

} // namespace eng::pipe
