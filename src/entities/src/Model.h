#pragma once

#include "../../base/src/PolygonVertexOnly.h"
#include <vector>
#include "../../matrix/src/Matrix.h"

namespace eng::ent{

template<typename T> concept polygon = std::is_same_v<T, PolygonVertexOnly>
    || std::is_same_v<T, TriangleVertexOnly>
    || std::is_same_v<T, QuadVertexOnly>;

template<typename PolygonType>
requires polygon<PolygonType>
class Model {
    [[nodiscard]]
    inline mtr::Matrix getModelMatrix() const noexcept{return modelMatrix;}

    void rotateX(floating degree);
    void rotateY(floating degree);
    void rotateZ(floating degree);
private:
    std::vector<PolygonType> polygons;
    mtr::Matrix modelMatrix;
};

} // namespace eng::ent
