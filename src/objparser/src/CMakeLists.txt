add_library(objparser STATIC ObjParser.h ParsingFunctions.cpp ParsingFunctions.h)
target_link_libraries(objparser PRIVATE options warnings)
