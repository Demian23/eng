#pragma once

#include "../../vector/src/DimensionalVector.h"
#include <optional>
#include <vector>

namespace eng::obj {

using Vertex = vec::FourDimensionalVector;
using Normal = vec::ThreeDimensionalVector;

enum { MinimalPolygonComponents = 3 };

struct PolygonIndexes {
    numeric vertexIndex;
    numeric normalIndex;
};

struct PolygonComponent final {
    Vertex vertex;
    Normal normal;
};

struct Polygon final : std::vector<PolygonComponent> {
    Polygon(size_t size) : std::vector<PolygonComponent>(size) {}
};


} // namespace eng::obj
