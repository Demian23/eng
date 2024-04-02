add_executable(executive main.cpp
        RGBArray.h
        ScreenDrawer.h
        ScreenDrawer.cpp
)

find_library(FLTK NAME fltk)
target_link_libraries(executive PRIVATE options warnings objparser mtr pipe FLTK)
