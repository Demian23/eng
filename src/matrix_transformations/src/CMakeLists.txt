enable_testing()
add_library(mtr STATIC Matrix.h Matrix.cpp
        Move.h
        Scale.h
        Rotate.h)
target_link_libraries(mtr PRIVATE base)