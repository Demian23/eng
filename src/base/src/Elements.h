#pragma once

#include "../../vector/src/DimensionalVector.h"
#include <vector>

namespace eng {

using Vertex = vec::FourDimensionalVector;
using Normal = vec::ThreeDimensionalVector;

struct PolygonIndexes {
    numeric vertexIndex;
    numeric normalIndex;
};

struct PolygonComponent final {
    Vertex vertex;
    Normal normal;
};

struct Polygon final : std::vector<PolygonComponent> {
    explicit Polygon(size_t size) : std::vector<PolygonComponent>(size) {}
};

} // namespace eng
