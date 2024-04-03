enable_testing()
add_library(ent STATIC Camera.cpp Camera.h
        CameraProjection.cpp
        CameraProjection.h
        Model.h
        Model.cpp
        Light.h)
target_link_libraries(ent PUBLIC options warnings vec)
