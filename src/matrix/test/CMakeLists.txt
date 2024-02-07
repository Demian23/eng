enable_testing()

add_executable(mtrtest mtrtest.cpp)
add_test(NAME MatrixTransformations COMMAND mtrtest)

target_link_libraries(mtrtest PRIVATE mtr)
