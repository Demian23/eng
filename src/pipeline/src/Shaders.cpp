#include "Shaders.h"

namespace eng::shader {

vec::Vec3F NormalInterpolation::operator()([[maybe_unused]] floating u,
                                           [[maybe_unused]] floating v,
                                           [[maybe_unused]] floating w,
                                           Triangle triangle) const noexcept
{
    auto aNormal = *(_normals + triangle[0].normalOffset);
    auto bNormal = *(_normals + triangle[1].normalOffset);
    auto cNormal = *(_normals + triangle[2].normalOffset);
    auto normalInPoint =
        Normal{aNormal[0] * u + bNormal[0] * v + cNormal[0] * w,
               aNormal[1] * u + bNormal[1] * v + cNormal[1] * w,
               aNormal[2] * u + bNormal[2] * v + cNormal[2] * w}
            .normalize();
    return normalInPoint;
}

vec::Vec3F NormalForTriangle::operator()(Triangle triangle) const noexcept
{
    auto a = (*(_vertices + triangle[0].vertexOffset)).trim<3>();
    auto b = (*(_vertices + triangle[1].vertexOffset)).trim<3>();
    auto c = (*(_vertices + triangle[2].vertexOffset)).trim<3>();
    return vec::cross(c - a, b - a).normalize();
}

vec::Vec3F NormalForTriangleWithCaching::operator()(
    [[maybe_unused]] floating u, [[maybe_unused]] floating v,
    [[maybe_unused]] floating w, Triangle triangle) const noexcept
{
    if (_previousTriangle == triangle) {
        return _previousNormal;
    } else {
        _previousTriangle = triangle;
        _previousNormal = NormalForTriangle::operator()(_previousTriangle);
        return _previousNormal;
    }
}

[[nodiscard]] vec::Vec3<uint8_t> TextureProcessing::getRGB(
    [[maybe_unused]] floating u, [[maybe_unused]] floating v,
    [[maybe_unused]] floating w, Triangle triangle) const noexcept
{
    auto aCoordinate =
        *(_texturesCoordinates + triangle[0].textureCoordinatesOffset);
    auto bCoordinate =
        *(_texturesCoordinates + triangle[1].textureCoordinatesOffset);
    auto cCoordinate =
        *(_texturesCoordinates + triangle[2].textureCoordinatesOffset);

    auto aX = aCoordinate[0] * static_cast<floating>(_width) -
              static_cast<floating>(0.5);
    auto aY = (1 - aCoordinate[1]) * static_cast<floating>(_height) -
              static_cast<floating>(0.5);
    auto bX = bCoordinate[0] * static_cast<floating>(_width) -
              static_cast<floating>(0.5);
    auto bY = (1 - bCoordinate[1]) * static_cast<floating>(_height) -
              static_cast<floating>(0.5);
    auto cX = cCoordinate[0] * static_cast<floating>(_width) -
              static_cast<floating>(0.5);
    auto cY = (1 - cCoordinate[1]) * static_cast<floating>(_height) -
              static_cast<floating>(0.5);

    auto aW = (*(_vertices + triangle[0].vertexOffset))[3];
    auto bW = (*(_vertices + triangle[1].vertexOffset))[3];
    auto cW = (*(_vertices + triangle[2].vertexOffset))[3];

    auto x = std::clamp(
        static_cast<uint64_t>((aX * u / aW + bX * v / bW + cX * w / cW) /
                              (u / aW + v / bW + w / cW)),
        static_cast<uint64_t>(0), static_cast<uint64_t>(_width));
    auto y = std::clamp(
        static_cast<uint64_t>((aY * u / aW + bY * v / bW + cY * w / cW) /
                              (u / aW + v / bW + w / cW)),
        static_cast<uint64_t>(0), static_cast<uint64_t>(_height));

    auto offset = (y * _width + x) * _channel; // maybe round
    auto rgbPtr = _textureData + offset;

    auto r = static_cast<uint8_t>(rgbPtr[0]);
    auto g = static_cast<uint8_t>(rgbPtr[1]);
    auto b = static_cast<uint8_t>(rgbPtr[2]);
    return vec::Vec3<uint8_t>{r, g, b};
}
} // namespace eng::shader
