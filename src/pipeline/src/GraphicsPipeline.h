#pragma once

#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"
#include "../../entities/src/Model.h"
#include "../../rasterization/src/rast.h"

namespace eng::pipe {

class GraphicsPipeline final {
public:
    GraphicsPipeline(ent::Model &model, ent::Camera &camera,
                     ent::CameraProjection &projection)
        : _model(model), _camera(camera), _projection(projection), _zBuffer{},
          _xSize{}, _projectionType{ent::ProjectionType::Perspective}
    {}

    [[nodiscard]] std::vector<Vertex>
    applyVertexTransformations(int minX, int maxX, int minY,
                               int maxY) const noexcept;
    [[nodiscard]] ent::Model::polygonIteratorType
    trianglesBegin() const noexcept;
    [[nodiscard]] ent::Model::polygonIteratorType trianglesEnd() const noexcept;

    ent::ProjectionType getProjectionType() const noexcept;
    void setProjectionType(ent::ProjectionType newProjectionType) noexcept;

    void setZBufferSize(uint32_t bufferSize, uint32_t xSize);

    // with zBuffer check and back-face culling
    template <typename Out>
    void rasterModel(int minX, int maxX, int minY, int maxY, Out out)
    {
        auto verticesCopy = applyVertexTransformations(minX, maxX, minY, maxY);
        std::fill(_zBuffer.begin(), _zBuffer.end(), 1);
        auto copyIterator = verticesCopy.cbegin();
        std::for_each(
            trianglesBegin(), trianglesEnd(),
            [=, cameraEye = _camera.getEye(), zBufferIter = _zBuffer.begin(),
             zBufferSize = _zBuffer.size(),
             xSize = _xSize](auto &&polygon) mutable {
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
                    auto zBufferChecker = [=, azInverse = 1 / a[2],
                                           bzInverse = 1 / b[2],
                                           czInverse =
                                               1 / c[2]](uint32_t x, uint32_t y,
                                                         floating u, floating v,
                                                         floating w) {
                        auto z =
                            1 / (azInverse * u + bzInverse * v + czInverse * w);
                        auto point = (zBufferIter + y * xSize + x);
                        if (y * xSize + x < zBufferSize && z < *point) {
                            out(x, y, u, v, w);
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
    std::vector<floating> _zBuffer;
    uint32_t _xSize;
    ent::ProjectionType _projectionType;
};

} // namespace eng::pipe
