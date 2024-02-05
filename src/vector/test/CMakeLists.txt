enable_testing()

add_executable(vectest vectortest.cpp)
add_test(NAME Vector COMMAND vectest)

target_link_libraries(vectest PRIVATE vec)
