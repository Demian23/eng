#pragma once

#include "../../vector/src/DimensionalVector.h"
#include <vector>

namespace eng {

using Vertex = vec::Vec4F;
using Normal = vec::Vec3F;
using TextureCoord = vec::Vec3F;
struct PolygonComponent {
    Vertex *vertex{};
    Normal *normal{};
    TextureCoord *textureCoordinates{};
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
