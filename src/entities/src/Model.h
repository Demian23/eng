#pragma once

#include "../../base/src/PolygonVertexOnly.h"
#include "../../matrix/src/Move.h"
#include "../../matrix/src/Rotate.h"
#include "../../matrix/src/Scale.h"
#include <vector>

namespace eng::ent {

/*
 * Model is class representing one model object with all normals, color rules
 * texture coordinates. Polygons always represented as triangles for
 * rasterization polygons have refs to vertex must be way work with triangles
 * only (then when reading polygon will be divided) and with original polygons
 * (needs reconfigure) color mode is also optional as texture mode
 */

class Model final {
public:
    using polygonIteratorType = std::vector<Triangle>::const_iterator;
    using vertexIteratorType = std::vector<Vertex>::const_iterator;

    Model(std::vector<Vertex> &&vertices, std::vector<Triangle> &&polygons,
          std::vector<Normal> &&normals = {},
          std::vector<TextureCoord> &&textureCoords = {})
        : _vertices(std::move(vertices)), _normals(std::move(normals)),
          _textureCoords(std::move(textureCoords)), _triangles(polygons),
          modelMatrix(mtr::Matrix::createIdentityMatrix())
    {}

    void addModelTransformation(mtr::Matrix transformation) noexcept;

    [[nodiscard]] mtr::Matrix getModelMatrix() const noexcept;

    void rotateX(floating degree) noexcept;

    void rotateY(floating degree) noexcept;

    void rotateZ(floating degree) noexcept;

    void scale(floating on) noexcept;

    void move(vec::Vec3F where) noexcept;

    void clearModelMatrix() noexcept;

    [[nodiscard]] polygonIteratorType trianglesBegin() const noexcept;

    [[nodiscard]] polygonIteratorType trianglesEnd() const noexcept;

    [[nodiscard]] vertexIteratorType verticesBegin() const noexcept;

    [[nodiscard]] vertexIteratorType verticesEnd() const noexcept;

    [[nodiscard]] bool empty() const noexcept;

private:
    std::vector<Vertex> _vertices;
    std::vector<Normal> _normals;
    std::vector<TextureCoord> _textureCoords;
    std::vector<Triangle> _triangles;
    mtr::Matrix modelMatrix;
};

} // namespace eng::ent
