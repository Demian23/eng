#pragma once

#include "../../vector/src/DimensionalVector.h"
#include <cstdint>
#include <vector>

namespace eng {

using Vertex = vec::Vec4F;
using Normal = vec::Vec3F;
using TextureCoord = vec::Vec3F;
using ScreenPixel = vec::Vec3<uint8_t>;

struct PolygonComponent {
    constexpr static ssize_t invalidOffset = -1;
    ssize_t vertexOffset{invalidOffset};
    ssize_t normalOffset{invalidOffset};
    ssize_t textureCoordinatesOffset{invalidOffset};
};

inline bool operator==(const PolygonComponent a, const PolygonComponent b)
{
    return a.vertexOffset == b.vertexOffset;
}

struct PolygonIndexes {
    integral vertexIndex;
    integral normalIndex;
    integral textureCoordinatesIndex;
};

using Polygon = std::vector<PolygonComponent>; // or pointer
using Triangle = std::array<PolygonComponent, 3>;

} // namespace eng
