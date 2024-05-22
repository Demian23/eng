add_executable(executive main.cpp
        ScreenDrawer.h
        ScreenDrawer.cpp
)

target_link_libraries(executive PRIVATE options warnings objparser mtr pipe fltk fltk_images)
