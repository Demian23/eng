enable_testing()
add_library(mtr STATIC Matrix.h Matrix.cpp
        MoveMatrix.h
        ScaleMatrix.h)
target_link_libraries(mtr PRIVATE base)