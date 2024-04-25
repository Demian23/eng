#include "Model.h"

namespace eng::ent {

void Model::reset(std::vector<Vertex> &&vertices,
                  std::vector<Triangle> &&polygons,
                  std::vector<Normal> &&normals,
                  std::vector<TextureCoord> &&textureCoords)
{
    _vertices = std::move(vertices);
    _triangles = std::move(polygons);
    _normals = std::move(normals);
    _textureCoords = std::move(textureCoords);
}
void Model::addModelTransformation(mtr::Matrix transformation) noexcept
{
    modelMatrix = transformation * modelMatrix;
}

[[nodiscard]] mtr::Matrix Model::getModelMatrix() const noexcept
{
    return modelMatrix;
}

void Model::rotateX(floating degree) noexcept
{
    modelMatrix = mtr::Matrix::getRotateX(degree) * modelMatrix;
}

void Model::rotateY(floating degree) noexcept
{
    modelMatrix = mtr::Matrix::getRotateY(degree) * modelMatrix;
}

void Model::rotateZ(floating degree) noexcept
{
    modelMatrix = mtr::Matrix::getRotateZ(degree) * modelMatrix;
}

void Model::scale(floating on) noexcept
{
    modelMatrix = mtr::Matrix::getScale({on, on, on}) * modelMatrix;
}

void Model::move(vec::Vec3F where) noexcept
{
    modelMatrix = mtr::Matrix::getMove(where) * modelMatrix;
}

void Model::clearModelMatrix() noexcept
{
    modelMatrix = mtr::Matrix::createIdentityMatrix();
}

[[nodiscard]] vec::Vec3F Model::getAlbedo() const noexcept { return _albedo; }
void Model::setAlbedo(vec::Vec3F newAlbedo) noexcept { _albedo = newAlbedo; }

void Model::setDiffuseMap(std::unique_ptr<Fl_RGB_Image>&& diffuse){
    _diffuseMap = std::move(diffuse);
}
void Model::setSpecularMap(std::unique_ptr<Fl_RGB_Image>&& specular){
    _specularMap = std::move(specular);
}
void Model::setNormalMap(std::unique_ptr<Fl_RGB_Image>&& normal){
    _normalMap = std::move(normal);
}

} // namespace eng::ent