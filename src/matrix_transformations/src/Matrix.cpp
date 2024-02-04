#include "Matrix.h"

namespace eng::mtr {

FourDimensionalVector
Matrix::operator*(FourDimensionalVector vector) const noexcept
{
    FourDimensionalVector result;
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

} // namespace eng::mtr
