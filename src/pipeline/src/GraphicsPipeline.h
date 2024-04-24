#pragma once

#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"
#include "../../entities/src/Light.h"
#include "../../entities/src/Model.h"
#include "../../algorithm/src/alg.h"

#include <numbers>

namespace eng::pipe {

template <typename T>
concept Shader = requires(T &&shader, uint32_t x, uint32_t y, floating u,
                          floating v, floating w, Triangle triangle) {
    {
    shader(x, y, u, v, w, triangle)
    } -> std::same_as<void>;
};

class GraphicsPipeline final {
public:
    GraphicsPipeline(ent::Model &model, ent::Camera &camera,
                     ent::CameraProjection &projection);

    [[nodiscard]] std::vector<Vertex>
    applyVertexTransformations(int minX, int maxX, int minY,
                               int maxY) const noexcept;

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

    template <Shader ShaderCallable>
    void rasterize(std::vector<Vertex>::const_iterator vc,
                   ShaderCallable &&shader)
    {
        std::fill(_zBuffer.begin(), _zBuffer.end(), 1);
        auto zBufferCheck = [zIter = _zBuffer.begin(), xSize = _xSize,
                             zSize = _zBuffer.size()](uint32_t x, uint32_t y,
                                                      floating z) {
            auto index = y * xSize + x;
            auto point = zIter + index;
            auto res = index < zSize && z < *point;
            if (res)
                *point = z;
            return res;
        };
        std::for_each(
            _model.trianglesBegin(), _model.trianglesEnd(),
            [=, zBuffer = std::move(zBufferCheck),
             svIt = _model.verticesBegin(), cvIt = vc,
             mMatrix = _model.getModelMatrix(),
             cEye = _camera.getEye()](auto &&triangle) {
                auto aInWorldSpace =
                    (mMatrix * *(svIt + triangle[0].vertexOffset)).template trim<3>();
                auto bInWorldSpace =
                    (mMatrix * *(svIt + triangle[1].vertexOffset)).template trim<3>();
                auto cInWorldSpace =
                    (mMatrix * *(svIt + triangle[2].vertexOffset)).template trim<3>();
                auto tNormal = vec::cross(cInWorldSpace - aInWorldSpace,
                                          bInWorldSpace - aInWorldSpace);
                auto eyeDirection = aInWorldSpace - cEye;
                auto normalDotEye = eyeDirection * tNormal;
                if (normalDotEye >= 0) {
                    auto a = (cvIt + triangle[0].vertexOffset)->template trim<3>();
                    auto b = (cvIt + triangle[1].vertexOffset)->template trim<3>();
                    auto c = (cvIt + triangle[2].vertexOffset)->template trim<3>();
                    alg::barycentricCoordinates(
                        a, b, c, zBuffer,
                        [=](uint32_t x, uint32_t y, floating u, floating v,
                            floating w) { shader(x, y, u, v, w, triangle); });
                }
            });
    }
private:
    ent::Model &_model;
    ent::Camera &_camera;
    ent::CameraProjection &_projection;
    std::vector<floating> _zBuffer;
    uint32_t _xSize;
    ent::ProjectionType _projectionType;
};

} // namespace eng::pipe
