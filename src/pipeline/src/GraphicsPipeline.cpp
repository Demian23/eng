#include "GraphicsPipeline.h"
#include "../../matrix/src/Viewport.h"

namespace eng::pipe {

[[nodiscard]] std::vector<Vertex>
GraphicsPipeline::applyVertexTransformations(int minX, int maxX, int minY,
                                             int maxY) const noexcept
{
    std::vector<Vertex> copy{_model.verticesBegin(), _model.verticesEnd()};
    auto transformationMatrix =
        eng::mtr::Viewport{
            static_cast<floating>(minX), static_cast<floating>(maxX),
            static_cast<floating>(minY), static_cast<floating>(maxY)} *
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

[[nodiscard]] ent::Model::polygonIteratorType
GraphicsPipeline::trianglesBegin() const noexcept
{
    return _model.trianglesBegin();
}
[[nodiscard]] ent::Model::polygonIteratorType
GraphicsPipeline::trianglesEnd() const noexcept
{
    return _model.trianglesEnd();
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