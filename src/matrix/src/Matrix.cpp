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

} // namespace eng::mtr
