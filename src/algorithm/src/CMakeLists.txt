enable_testing()
add_library(alg STATIC alg.h alg.cpp)
target_link_libraries(alg PRIVATE base)