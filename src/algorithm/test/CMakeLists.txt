enable_testing()

add_executable(algtest algtest.cpp)
add_test(NAME Algorithms COMMAND algtest)

target_link_libraries(algtest PRIVATE alg)
