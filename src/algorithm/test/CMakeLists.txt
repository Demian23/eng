enable_testing()

add_executable(algtest ../src/alg.h algtest.cpp)
add_test(NAME Algorithms COMMAND algtest)
