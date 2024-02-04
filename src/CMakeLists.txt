enable_testing()
add_subdirectory(objparser)
add_subdirectory(matrix_transformations)

add_library(base INTERFACE)
#target_include_directories(base INTERFACE ${CMAKE_SOURCE_DIR}/src/config)
target_link_libraries(base INTERFACE options warnings)
target_sources(base PUBLIC eng.h)
