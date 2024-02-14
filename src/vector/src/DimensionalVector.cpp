#include "DimensionalVector.h"
namespace eng::vec {

ThreeDimensionalVector
vectorMultiplication(const ThreeDimensionalVector &a,
                     const ThreeDimensionalVector &b) noexcept
{
    ThreeDimensionalVector result;
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    return result;
}
FourDimensionalVector cartesianToHomogeneous(const ThreeDimensionalVector &a,
                                             floating w = 1) noexcept
{
    return {a[0], a[1], a[2], w};
}

ThreeDimensionalVector
homogeneousToCartesian(const FourDimensionalVector &a) noexcept
{
    auto w = a[3];
    if (w == 0) {
        return {a[0], a[1], a[2]};
    } else {
        return {a[0] / w, a[1] / w, a[2] / w};
    }
}

ThreeDimensionalVector sphericalToCartesian(
    const ThreeDimensionalVector &vectorInSphericalNotation) noexcept
{
    auto radialLine = vectorInSphericalNotation[0],
         polarAngleInRad = vectorInSphericalNotation[1],
         azimuthalAngleInRad = vectorInSphericalNotation[2];
    return {
        radialLine * std::sin(polarAngleInRad) * std::cos(azimuthalAngleInRad),
        radialLine * std::sin(polarAngleInRad) * std::sin(azimuthalAngleInRad),
        radialLine * std::cos(polarAngleInRad)};
}
} // namespace eng::vec
