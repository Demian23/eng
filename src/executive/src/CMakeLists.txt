add_executable(executive main.cpp
        RGBArrayInserter.h
        RGBArray.h
        MonoColorDrawer.h)

find_library(FLTK NAME fltk)
target_link_libraries(executive PRIVATE options warnings objparser mtr FLTK)
