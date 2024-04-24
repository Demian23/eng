#pragma once

#include "../../base/src/Elements.h"
#include "../../matrix/src/Matrix.h"
#include "../../vector/src/DimensionalVector.h"
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
    using normalsIteratorType = std::vector<Normal>::const_iterator;

    static constexpr vec::Vec3F defaultAlbedo = {0.18f, 0.18f, 0.18f};
    Model():_vertices{}, _normals{}, _textureCoords{}, _triangles{}, _albedo{defaultAlbedo}, modelMatrix{mtr::Matrix::createIdentityMatrix()}{}
    Model(std::vector<Vertex> &&vertices, std::vector<Triangle> &&polygons,
          std::vector<Normal> &&normals = {},
          std::vector<TextureCoord> &&textureCoords = {},
          vec::Vec3F albedo = defaultAlbedo)
        : _vertices(std::move(vertices)), _normals(std::move(normals)),
          _textureCoords(std::move(textureCoords)), _triangles(polygons),
          _albedo{albedo}, modelMatrix(mtr::Matrix::createIdentityMatrix())
    {}

    void reset(std::vector<Vertex> &&vertices, std::vector<Triangle> &&polygons,
          std::vector<Normal> &&normals,
          std::vector<TextureCoord> &&textureCoords);

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

    [[nodiscard]] normalsIteratorType normalsBegin() const noexcept;

    [[nodiscard]] normalsIteratorType normalsEnd() const noexcept;

    [[nodiscard]] inline auto texturesBegin() const noexcept{
        return _textureCoords.cbegin();
    }
    [[nodiscard]] inline auto texturesEnd() const noexcept{
        return _textureCoords.cend();
    }

    [[nodiscard]] inline vec::Vec3F getAlbedo() const noexcept
    {
        return _albedo;
    }
    inline void setAlbedo(vec::Vec3F newAlbedo) noexcept
    {
        _albedo = newAlbedo;
    }

    [[nodiscard]] bool empty() const noexcept;

private:
    std::vector<Vertex> _vertices;
    std::vector<Normal> _normals;
    std::vector<TextureCoord> _textureCoords;
    std::vector<Triangle> _triangles;
    vec::Vec3F _albedo;
    mtr::Matrix modelMatrix;
};

} // namespace eng::ent
