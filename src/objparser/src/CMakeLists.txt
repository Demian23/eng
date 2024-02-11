enable_testing()
add_library(objparser STATIC Elements.h ObjParser.h ParsingFunctions.cpp
        PolygonVertexOnly.h
        ParsingFunctions.h
)