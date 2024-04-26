#pragma once

#include "../../base/src/Elements.h"
#include "../../matrix/src/Matrix.h"
#include "../../vector/src/DimensionalVector.h"
#include "FL/Fl_RGB_Image.H"
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
    static constexpr vec::Vec3F defaultAlbedo = {0.18f, 0.18f, 0.18f};

    Model()
        : _vertices{}, _normals{}, _textureCoords{}, _triangles{},
          _diffuseMap(), _normalMap(), _specularMap(), _albedo{defaultAlbedo},
          modelMatrix{mtr::Matrix::createIdentityMatrix()}
    {}
    Model(std::vector<Vertex> &&vertices, std::vector<Triangle> &&polygons,
          std::vector<Normal> &&normals = {},
          std::vector<TextureCoord> &&textureCoords = {},
          vec::Vec3F albedo = defaultAlbedo,
          std::unique_ptr<Fl_RGB_Image> &&diffuseMap = nullptr,
          std::unique_ptr<Fl_RGB_Image> &&normalMap = nullptr,
          std::unique_ptr<Fl_RGB_Image> &&specularMap = nullptr)
        : _vertices(std::move(vertices)), _normals(std::move(normals)),
          _textureCoords(std::move(textureCoords)), _triangles(polygons),
          _diffuseMap{std::move(diffuseMap)}, _normalMap{std::move(normalMap)},
          _specularMap{std::move(specularMap)}, _albedo{albedo},
          modelMatrix{mtr::Matrix::createIdentityMatrix()}
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

    [[nodiscard]] vec::Vec3F getAlbedo() const noexcept;
    void setAlbedo(vec::Vec3F newAlbedo) noexcept;

    void setDiffuseMap(std::unique_ptr<Fl_RGB_Image> &&diffuse);
    void setSpecularMap(std::unique_ptr<Fl_RGB_Image> &&specular);
    void setNormalMap(std::unique_ptr<Fl_RGB_Image> &&normal);
    const auto &getDiffuseMap() const noexcept { return _diffuseMap; }
    const auto &getNormalMap() const noexcept { return _normalMap; }
    const auto &getSpecularMap() const noexcept { return _specularMap; }

    [[nodiscard]] inline auto trianglesBegin() const noexcept
    {
        return _triangles.cbegin();
    }

    [[nodiscard]] inline auto trianglesEnd() const noexcept
    {
        return _triangles.cend();
    }

    [[nodiscard]] inline auto verticesBegin() const noexcept
    {
        return _vertices.cbegin();
    }

    [[nodiscard]] inline auto verticesEnd() const noexcept
    {
        return _vertices.cend();
    }

    [[nodiscard]] inline auto normalsBegin() const noexcept
    {
        return _normals.cbegin();
    }

    [[nodiscard]] inline auto normalsEnd() const noexcept
    {
        return _normals.cend();
    }
    [[nodiscard]] inline auto textureCoordsBegin() const noexcept
    {
        return _textureCoords.cbegin();
    }

    [[nodiscard]] inline auto textureCoordsEnd() const noexcept
    {
        return _textureCoords.cend();
    }

private:
    std::vector<Vertex> _vertices;
    std::vector<Normal> _normals;
    std::vector<TextureCoord> _textureCoords;
    std::vector<Triangle> _triangles;
    std::unique_ptr<Fl_RGB_Image> _diffuseMap;
    std::unique_ptr<Fl_RGB_Image> _normalMap;
    std::unique_ptr<Fl_RGB_Image> _specularMap;
    vec::Vec3F _albedo;
    mtr::Matrix modelMatrix;
};

} // namespace eng::ent
