#pragma once

#include "Elements.h"

namespace eng::obj {

struct PolygonVertexOnly final : std::vector<Vertex> {
    explicit PolygonVertexOnly(size_t size) : std::vector<Vertex>(size) {}
    static PolygonVertexOnly makePolygon(std::string_view strRep,
                                         const std::vector<Vertex> &allVertex);
};

} // namespace eng::obj