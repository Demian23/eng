#include "DimensionalVector.h"
namespace eng::vec {

ThreeDimensionalVector vectorMultiplication(const ThreeDimensionalVector &a,
                                            const ThreeDimensionalVector &b) noexcept
{
    ThreeDimensionalVector result;
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    return result;
}
FourDimensionalVector threeDimensionsToFour(const ThreeDimensionalVector& a, floating w) noexcept
{
    return {a[0], a[1], a[2], w};
}

}