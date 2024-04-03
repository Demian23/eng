enable_testing()
add_library(mtr STATIC Matrix.h Matrix.cpp
)
target_link_libraries(mtr PUBLIC options warnings vec)