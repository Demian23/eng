#include "Matrix.h"

namespace eng::mtr {

Vec4F Matrix::operator*(Vec4F vector) const noexcept
{
    Vec4F result;
    auto resultIter = result.begin();
    for (const auto &row : matrix_) {
        floating sum{};
        auto vectorIter = vector.begin();
        for (auto column_element : row) {
            sum += column_element * *vectorIter;
            ++vectorIter;
        }
        *resultIter = sum;
        ++resultIter;
    }
    return result;
}
Matrix Matrix::operator*(const Matrix &matrix) const noexcept
{
    MatrixRepresentation result;
    for (unsigned i = 0; i < MatrixDimension; i++) {
        for (unsigned k = 0; k < MatrixDimension; k++) {
            floating sum{};
            for (unsigned j = 0; j < MatrixDimension; j++) {
                sum += matrix_[i][j] * matrix.matrix_[j][k];
            }
            result[i][k] = sum;
        }
    }
    return Matrix{result};
}

bool Matrix::operator==(const Matrix &matrix) const noexcept
{
    return matrix_ == matrix.matrix_;
}
bool Matrix::operator!=(const Matrix &matrix) const noexcept
{
    return matrix_ != matrix.matrix_;
}

Matrix Matrix::createIdentityMatrix()
{
    return Matrix{{{{1.0f, 0.0f, 0.0f, 0.0f},
                    {0.0f, 1.0f, 0.0f, 0.0f},
                    {0.0f, 0.0f, 1.0f, 0.0f},
                    {0.0f, 0.0f, 0.0f, 1.0f}}}};
}

Matrix Matrix::getMove(Vec3F moveVector)
{
    Matrix result{};
    for (unsigned i = 0; i < MatrixDimension - 1; i++) {
        result.matrix_[i][i] = 1.0f;
    }
    for (unsigned i = 0; i < MatrixDimension - 1; i++) {
        result.matrix_[i][3] = moveVector[i];
    }
    return result;
}

Matrix Matrix::getRotateX(floating degree)
{
    Matrix result{};
    result.matrix_[0][0] = 1.0f;
    result.matrix_[1][1] = result.matrix_[2][2] =
        std::cos(degreeToRadian(degree));
    result.matrix_[2][1] = std::sin(degreeToRadian(degree));
    result.matrix_[1][2] = -result.matrix_[2][1];
    return result;
}
Matrix Matrix::getRotateY(floating degree)
{
    Matrix result{};
    result.matrix_[1][1] = 1.0f;
    result.matrix_[0][0] = result.matrix_[2][2] =
        std::cos(degreeToRadian(degree));
    result.matrix_[0][2] = std::sin(degreeToRadian(degree));
    result.matrix_[2][0] = -result.matrix_[0][2];
    return result;
}
Matrix Matrix::getRotateZ(floating degree)
{
    Matrix result{};
    result.matrix_[2][2] = 1.0f;
    result.matrix_[0][0] = result.matrix_[1][1] =
        std::cos(degreeToRadian(degree));
    result.matrix_[1][0] = std::sin(degreeToRadian(degree));
    result.matrix_[0][1] = -result.matrix_[1][0];
    return result;
}

Matrix Matrix::getScale(Vec3F scaleVector)
{
    Matrix result{};
    for (unsigned i = 0; i < scaleVector.size(); i++) {
        result.matrix_[i][i] = scaleVector[i];
    }
    return result;
}

Matrix Matrix::getView(Vec4F eye, Vec4F target, Vec4F up)
{
    Matrix result{};
    auto zAxis = (eye - target).normalize();
    auto xAxis =
        (vec::vectorMultiplicationForHomogeneous(zAxis, up)).normalize();
    auto yAxis = up; // vec::vectorMultiplication(xAxis, zAxis); // ?

    xAxis[3] = -(xAxis * eye);
    yAxis[3] = -(yAxis * eye);
    zAxis[3] = -(zAxis * eye);

    result.matrix_[0] = xAxis;
    result.matrix_[1] = yAxis;
    result.matrix_[2] = zAxis;
    return result;
}

Matrix Matrix::getViewport(floating xMin, floating xMax, floating yMin,
                           floating yMax)
{
    Matrix result{};
    auto width = (xMax - xMin);
    auto height = (yMax - yMin);
    result.matrix_[0][0] = width / 2;
    result.matrix_[1][1] = -height / 2;
    result.matrix_[2][2] = 1;
    result.matrix_[0][3] = xMin + width / 2;
    result.matrix_[1][3] = yMin + height / 2;
    return result;
}

Matrix Matrix::getOrthographicProjection(floating width, floating height,
                                         floating zMin, floating zMax)
{
    Matrix result{};
    result.matrix_[0][0] = 2.0f / width;
    result.matrix_[1][1] = 2.0f / height;
    result.matrix_[2][2] = 1.0f / (zMin - zMax);
    result.matrix_[2][3] = zMin / (zMin - zMax);
    return result;
}

Matrix Matrix::getPerspectiveProjection(floating xMin, floating xMax,
                                        floating yMin, floating yMax,
                                        floating zMin, floating zMax)
{
    Matrix result{};
    auto width = xMax - xMin;
    auto height = yMax - yMin;
    auto zDiff = zMin - zMax;
    result.matrix_[0][0] = (2 * zMin) / width;
    result.matrix_[1][1] = (2 * zMin) / height;
    result.matrix_[2][2] = zMax / zDiff;
    result.matrix_[2][3] = zMin * zMax / zDiff;
    result.matrix_[3][2] = -1;
    result.matrix_[3][3] = 0;
    return result;
}

Matrix Matrix::getPerspectiveProjectionWithAngle(floating degreeInRad,
                                                 floating aspect, floating zMin,
                                                 floating zMax)
{
    Matrix result{};
    auto zDiff = zMin - zMax;
    auto angle = static_cast<floating>(degreeInRad / 2);
    result.matrix_[0][0] =
        static_cast<floating>(1 / (aspect * std::tan(angle)));
    result.matrix_[1][1] = static_cast<floating>(1 / std::tan(angle));
    result.matrix_[2][2] = zMax / zDiff;
    result.matrix_[2][3] = zMin * zMax / zDiff;
    result.matrix_[3][2] = -1;
    result.matrix_[3][3] = 0;
    return result;
}
} // namespace eng::mtr
