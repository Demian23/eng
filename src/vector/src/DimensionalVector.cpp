#include "DimensionalVector.h"
namespace eng::vec {

FourDimensionalVector
vectorMultiplicationForHomogeneous(const FourDimensionalVector &a,
                                   const FourDimensionalVector &b) noexcept
{
    FourDimensionalVector result;
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    result[3] = 0;
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

ThreeDimensionalVector
cartesianToSpherical(const ThreeDimensionalVector &cartesianVector) noexcept
{
    ThreeDimensionalVector result{};
    auto vectorLen = cartesianVector.length();
    result[0] = vectorLen;
    result[1] = std::acos(cartesianVector[2] / vectorLen);
    result[2] = std::atan2(cartesianVector[1], cartesianVector[0]);
    return result;
}

} // namespace eng::vec
