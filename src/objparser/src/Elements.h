#pragma once

#include "../../eng.h"
#include <optional>
#include <vector>

namespace eng::obj {

using Vertex = FourDimensionalVector;
using Normal = ThreeDimensionalVector;

enum { MinimalPolygonComponents = 3 };

struct PolygonIndexes {
    numeric vertexIndex;
    numeric normalIndex;
};

struct PolygonComponent final {
    Vertex vertex;
    std::optional<Normal> normal;
};

struct Polygon final {
    Polygon(unsigned long size) : f{size} {}
    std::vector<PolygonComponent> f;
};

} // namespace eng::obj
