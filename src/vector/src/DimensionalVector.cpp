#include "DimensionalVector.h"
namespace eng::vec {

Vec4F vectorMultiplicationForHomogeneous(const Vec4F &a,
                                         const Vec4F &b) noexcept
{
    Vec4F result;
    result[0] = a[1] * b[2] - a[2] * b[1];
    result[1] = a[2] * b[0] - a[0] * b[2];
    result[2] = a[0] * b[1] - a[1] * b[0];
    result[3] = 0;
    return result;
}

Vec4F cartesianToHomogeneous(const Vec3F &a, floating w = 1) noexcept
{
    return {a[0], a[1], a[2], w};
}

Vec3F homogeneousToCartesian(const Vec4F &a) noexcept
{
    auto w = a[3];
    if (w == 0) {
        return {a[0], a[1], a[2]};
    } else {
        return {a[0] / w, a[1] / w, a[2] / w};
    }
}

Vec3F sphericalToCartesian(const Vec3F &vectorInSphericalNotation) noexcept
{
    auto radialLine = vectorInSphericalNotation[0],
         polarAngleInRad = vectorInSphericalNotation[1],    // O
        azimuthalAngleInRad = vectorInSphericalNotation[2]; // phi
    return {
        radialLine * std::sin(polarAngleInRad) * std::cos(azimuthalAngleInRad),
        radialLine * std::sin(polarAngleInRad) * std::sin(azimuthalAngleInRad),
        radialLine * std::cos(polarAngleInRad)};
}

Vec3F cartesianToSpherical(const Vec3F &cartesianVector) noexcept
{
    Vec3F result{};
    auto vectorLen = cartesianVector.length();
    result[0] = vectorLen;
    result[1] = std::acos(cartesianVector[2] / vectorLen);
    result[2] = std::atan2(cartesianVector[1], cartesianVector[0]);
    return result;
}

} // namespace eng::vec
