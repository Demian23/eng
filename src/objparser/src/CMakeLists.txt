enable_testing()
add_library(objparser STATIC Elements.h ObjParser.h ObjParser.cpp
        PolygonVertexOnly.h
        PolygonVertexOnly.cpp)
target_link_libraries(objparser PRIVATE base)
