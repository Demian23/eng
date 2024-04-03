#pragma once
#include "../../base/src/eng.h"
#include "../../vector/src/DimensionalVector.h"
#include <array>

namespace eng::mtr {

using eng::vec::Vec3F;
using eng::vec::Vec4F;

enum { MatrixDimension = 4 };

using MatrixRepresentation =
    std::array<std::array<floating, MatrixDimension>, MatrixDimension>;

class Matrix {
public:
    Matrix() : matrix_{}
    {
        matrix_[MatrixDimension - 1][MatrixDimension - 1] = 1.0f;
    }

    explicit Matrix(MatrixRepresentation sourceMatrix) : matrix_(sourceMatrix)
    {}

    [[nodiscard]] Vec4F operator*(Vec4F vector) const noexcept;

    [[nodiscard]] Matrix operator*(const Matrix &matrix) const noexcept;

    bool operator==(const Matrix &matrix) const noexcept;

    bool operator!=(const Matrix &matrix) const noexcept;

    static Matrix createIdentityMatrix();

    static Matrix getMove(Vec3F moveVector);

    static Matrix getRotateX(floating degree);

    static Matrix getRotateY(floating degree);

    static Matrix getRotateZ(floating degree);

    static Matrix getScale(Vec3F scaleVector);

    static Matrix getView(Vec4F eye, Vec4F target, Vec4F up);

    static Matrix getViewport(floating xMin, floating xMax, floating yMin,
                              floating yMax);

    static Matrix getOrthographicProjection(floating width, floating height,
                                            floating zMin, floating zMax);

    static Matrix getPerspectiveProjection(floating xMin, floating xMax,
                                           floating yMin, floating yMax,
                                           floating zMin, floating zMax);

    static Matrix getPerspectiveProjectionWithAngle(floating degreeInRad,
                                                    floating aspect,
                                                    floating zMin,
                                                    floating zMax);

protected:
    MatrixRepresentation matrix_;
};

} // namespace eng::mtr
