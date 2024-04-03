#include "Model.h"

namespace eng::ent {

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

Model::polygonIteratorType Model::trianglesBegin() const noexcept
{
    return _triangles.cbegin();
}

Model::polygonIteratorType Model::trianglesEnd() const noexcept
{
    return _triangles.cend();
}

[[nodiscard]] bool Model::empty() const noexcept { return _triangles.empty(); }

[[nodiscard]] Model::vertexIteratorType Model::verticesBegin() const noexcept
{
    return _vertices.cbegin();
}

[[nodiscard]] Model::vertexIteratorType Model::verticesEnd() const noexcept
{
    return _vertices.cend();
}

[[nodiscard]] Model::normalsIteratorType Model::normalsBegin() const noexcept
{
    return _normals.cbegin();
}

[[nodiscard]] Model::normalsIteratorType Model::normalsEnd() const noexcept
{
    return _normals.cend();
}

} // namespace eng::ent