#pragma once

#include "../../base/src/PolygonVertexOnly.h"
#include "../../matrix/src/Rotate.h"
#include "../../matrix/src/Scale.h"
#include <vector>

namespace eng::ent {

template <typename T>
concept PolygonType =
    std::is_same_v<T, PolygonVertexOnly> ||
    std::is_same_v<T, TriangleVertexOnly> || std::is_same_v<T, QuadVertexOnly>;

template <typename T>
    requires PolygonType<T>
class Model {
public:
    Model(std::vector<T> outerPolygons) 
        : polygons{std::move(outerPolygons)}, modelMatrix(mtr::Matrix::createIdentityMatrix()){}

    [[nodiscard]] inline mtr::Matrix getModelMatrix() const noexcept
    {
        return modelMatrix;
    }

    void rotateX(floating degree)
    {
        modelMatrix = mtr::RotateX{degree} * modelMatrix; 
    }

    void rotateY(floating degree)
    {
        modelMatrix = mtr::RotateY{degree} * modelMatrix; 
    }

    void rotateZ(floating degree)
    {
        modelMatrix = mtr::RotateZ{degree} * modelMatrix; 
    }

    void scale(floating on) noexcept{
        modelMatrix = mtr::Scale{{on, on, on}} * modelMatrix;
    }

    auto getPolygonsBegin() const noexcept {return polygons.cbegin();}
    auto getPolygonsEnd() const noexcept {return polygons.cend();}

    inline bool empty()const noexcept{return polygons.empty();}

private:
    std::vector<T> polygons;
    mtr::Matrix modelMatrix;
};

} // namespace eng::ent
