add_executable(executive main.cpp
        RGBArrayInserter.h
        RGBArray.h
        MonoColoredScreenDrawer.h)

find_library(FLTK NAME fltk)
target_link_libraries(executive PRIVATE options warnings objparser mtr pipe FLTK)
