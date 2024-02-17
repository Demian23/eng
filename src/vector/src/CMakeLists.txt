add_library(vec STATIC DimensionalVector.h DimensionalVector.cpp)
target_link_libraries(vec PRIVATE options warnings)