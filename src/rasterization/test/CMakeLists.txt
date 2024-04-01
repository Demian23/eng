enable_testing()
add_executable(rasttest ../src/rast.h rasttest.cpp)
target_link_libraries(rasttest PRIVATE vec)
add_test(NAME Rasterization COMMAND rasttest)
