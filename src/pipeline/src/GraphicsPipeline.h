#pragma once

#include "../../entities/src/Camera.h"
#include "../../entities/src/CameraProjection.h"
#include "../../entities/src/Model.h"

namespace eng::pipe {

class GraphicsPipeline final {
public:
    GraphicsPipeline(ent::Model &model, ent::Camera &camera,
                     ent::CameraProjection &projection)
        : _model(model), _camera(camera), _projection(projection),
          _projectionType{ent::ProjectionType::Perspective}
    {}

    [[nodiscard]] std::vector<Vertex>
    applyVertexTransformations(int minX, int maxX, int minY,
                               int maxY) const noexcept;
    [[nodiscard]] ent::Model::polygonIteratorType
    trianglesBegin() const noexcept;
    [[nodiscard]] ent::Model::polygonIteratorType trianglesEnd() const noexcept;

    ent::ProjectionType getProjectionType() const noexcept;
    void setProjectionType(ent::ProjectionType newProjectionType) noexcept;

private:
    ent::Model &_model;
    ent::Camera &_camera;
    ent::CameraProjection &_projection;
    ent::ProjectionType _projectionType;
};

} // namespace eng::pipe
