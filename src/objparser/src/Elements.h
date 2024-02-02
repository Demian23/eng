#pragma once

#include <vector>
#include <string_view>

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

enum{MinimalFaceComponents = 3};

struct FaceComponentEx final{
    Vertex* vertex; // TODO or smth as optional?
    TextureVertexCoordinates* textureCoord;
    Normal* normals;
};

struct FaceComponent final{
    Vertex* vertex; // TODO or smth as optional?
    Normal* normals;
};

struct FaceEx final{
    std::vector<FaceComponentEx> components{MinimalFaceComponents};
};

struct Face final{
    std::vector<FaceComponent> components{MinimalFaceComponents};
};


};
