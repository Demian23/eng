enable_testing()

add_executable(objparsertest objparsertest.cpp)
add_test(NAME ObjParser COMMAND objparsertest)

target_link_libraries(objparsertest objparser options warnings)
