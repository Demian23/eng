#pragma once

#include <optional>
#include <vector>

namespace obj{

using numeric = long;
using floating = float;

struct ThreeComponentVector final{
    floating x, y, z;
};

struct FourComponentVector final{
    floating x, y, z, w;
};

using Vertex = FourComponentVector;
using Normal = ThreeComponentVector;
using TextureVertexCoordinates = ThreeComponentVector;

enum{MinimalPolygonComponents= 3};

struct PolygonIndexes{
    numeric vertexIndex;
    numeric normalIndex;
};

struct PolygonComponent final{
    Vertex vertex; 
    std::optional<Normal> normal;
};

struct Polygon final{
    Polygon(unsigned long size) : f{size}{}
    std::vector<PolygonComponent> f;
};


};
