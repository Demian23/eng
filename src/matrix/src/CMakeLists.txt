enable_testing()
add_library(mtr STATIC Matrix.h Matrix.cpp
        Move.h
        Scale.h
        Rotate.h
        View.h
        OrthographicProjection.h
        Viewport.h
        PerspectiveProjection.h
        PerspectiveProjectionWithAngle.h)
target_link_libraries(mtr PUBLIC options warnings vec)