add_executable(executive main.cpp
        RGBArrayInserter.h
        RGBArray.h
        MonoColorDrawer.cpp
        MonoColorDrawer.h)

find_library(FLTK NAME fltk)
target_link_libraries(executive PRIVATE objparser mtr FLTK)
