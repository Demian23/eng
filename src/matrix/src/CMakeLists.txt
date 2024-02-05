enable_testing()
add_library(mtr STATIC Matrix.h Matrix.cpp
        Move.h
        Scale.h
        Rotate.h
        View.h
        OrthographicProjection.h
        Viewport.h)
target_link_libraries(mtr PRIVATE base vec)