#include "GraphicsPipeline.h"
#include "../../base/src/Elements.h"

namespace eng::pipe {

GraphicsPipeline::GraphicsPipeline(ent::Model &model, ent::Camera &camera,
                                   ent::CameraProjection &projection)
    : _model(model), _camera(camera), _projection(projection), _zBuffer{},
      _xSize{}, _projectionType{ent::ProjectionType::Perspective}
{}

[[nodiscard]] std::vector<Vertex>
GraphicsPipeline::applyVertexTransformations(int minX, int maxX, int minY,
                                             int maxY) const noexcept
{
    std::vector<Vertex> copy{_model.verticesBegin(), _model.verticesEnd()};
    auto transformationMatrix =
        mtr::Matrix::getViewport(
            static_cast<floating>(minX), static_cast<floating>(maxX),
            static_cast<floating>(minY), static_cast<floating>(maxY)) *
        _projection.getProjectionMatrix(_projectionType) *
        _camera.getViewMatrix() * _model.getModelMatrix();

    auto transformVertex = [=,
                            projection = this->_projectionType](auto &&vertex) {
        vertex = transformationMatrix * vertex;
        if (projection == eng::ent::ProjectionType::Perspective) {
            std::transform(vertex.begin(), vertex.end() - 1, vertex.begin(),
                           [w = *vertex.rbegin()](auto &&coord) {
                               // if you brave enough, you can erase branch here
                               // and leave coord /= w, but zero division is
                               // undefined behaviour, remember
                               return w > 0 ? coord /= w : coord;
                           });
        }

        auto x = vertex.begin();
        auto y = vertex.begin() + 1;
        *x = std::clamp(*x, static_cast<floating>(minX),
                        static_cast<floating>(maxX));
        *y = std::clamp(*y, static_cast<floating>(minY),
                        static_cast<floating>(maxY));

        return vertex;
    };
    std::transform(copy.begin(), copy.end(), copy.begin(), transformVertex);
    return copy;
}

void GraphicsPipeline::setZBufferSize(uint32_t bufferSize, uint32_t xSize)
{
    _zBuffer.resize(bufferSize);
    _xSize = xSize;
}

[[nodiscard]] ent::ProjectionType
GraphicsPipeline::getProjectionType() const noexcept
{
    return _projectionType;
}

void GraphicsPipeline::setProjectionType(
    ent::ProjectionType newProjectionType) noexcept
{
    _projectionType = newProjectionType;
}
} // namespace eng::pipe