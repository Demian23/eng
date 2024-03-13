enable_testing()

add_executable(algtest ../src/alg.h algtest.cpp)
target_link_libraries(algtest PRIVATE config)
add_test(NAME Algorithms COMMAND algtest)
