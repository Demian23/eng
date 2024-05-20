#pragma once
#include "../../base/src/Elements.h"
#include "../../pipeline/src/Shaders.h"
#include <cstddef>
#include <cstdint>
#include <vector>

// deferred rendering namespace
namespace eng::dr {

struct Fragment {
    vec::Vec3F point; // point in world space
    Normal normal;
    Specular specular;
    vec::Vec3F albedo;
};
// class that is used for first pass and then passed to object for illumination
template <shader::AlbedoProvider AlbedoProv, shader::NormalProvider NormalProv,
          shader::SpecularPropertiesProvider SpecularProv>
class GBuffer {
public:
    GBuffer(std::size_t gBufferSize, uint32_t xSize, AlbedoProv albedo,
            NormalProv normal, SpecularProv specular,
            shader::vci verticesInWorldSpace)
        : _gBuffer(gBufferSize), _albedo(albedo), _normal(normal),
          _specular(specular), _vertices(verticesInWorldSpace), _xSize(xSize)
    {}
    GBuffer(const GBuffer &) = delete;
    GBuffer &operator=(const GBuffer &) = delete;
    inline auto cbegin() const noexcept { return _gBuffer.cbegin(); }
    inline auto cend() const noexcept { return _gBuffer.cend(); }

    void operator()(uint32_t x, uint32_t y, floating u, floating v, floating w,
                    Triangle triangle) noexcept
    {
        uint64_t index = y * _xSize + x;
        if (index < _gBuffer.size()) [[likely]] {
            auto albedo = _albedo(u, v, w, triangle);
            auto normal = _normal(u, v, w, triangle);
            auto specular = _specular(u, v, w, triangle);
            auto aInWorldSpace = *(_vertices + triangle[0].vertexOffset);
            auto bInWorldSpace = *(_vertices + triangle[1].vertexOffset);
            auto cInWorldSpace = *(_vertices + triangle[2].vertexOffset);
            auto point =
                vec::Vec3F{aInWorldSpace[0] * u + bInWorldSpace[0] * v +
                               cInWorldSpace[0] * w,
                           aInWorldSpace[1] * u + bInWorldSpace[1] * v +
                               cInWorldSpace[1] * w,
                           aInWorldSpace[2] * u + bInWorldSpace[2] * v +
                               cInWorldSpace[2] * w};
            _gBuffer[index] = Fragment{point, normal, specular, albedo};
        }
    }

private:
    std::vector<Fragment> _gBuffer;
    AlbedoProv _albedo;
    NormalProv _normal;
    SpecularProv _specular;
    shader::vci _vertices;
    uint32_t _xSize;
};

} // namespace eng::dr
