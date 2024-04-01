#pragma once

#include "../../vector/src/DimensionalVector.h"
#include <vector>

namespace eng {

using Vertex = vec::Vec4F;
using Normal = vec::Vec3F;
using TextureCoord = vec::Vec3F;
// can't be used without corresponding to vertex, normal and so on collections
// collection should allow indexed access

struct PolygonComponent {
    constexpr static ssize_t invalidOffset = -1;
    ssize_t vertexOffset{};
    ssize_t normalOffset{};
    ssize_t textureCoordinatesOffset{};
};

struct PolygonIndexes {
    integral vertexIndex;
    integral normalIndex;
    integral textureCoordinatesIndex;
};

using Polygon = std::vector<PolygonComponent>; // or pointer
using Triangle = std::array<PolygonComponent, 3>;
using Quad = std::array<PolygonComponent, 4>;

} // namespace eng
