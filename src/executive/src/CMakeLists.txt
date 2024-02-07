add_executable(executive main.cpp
        RGBArrayInserter.h
        RGBArray.h
        MonoColorDrawer.cpp
        MonoColorDrawer.h)
target_link_directories(executive PUBLIC /usr/local/Cellar/fltk/1.3.9/lib/)
target_link_libraries(executive PRIVATE objparser mtr fltk)