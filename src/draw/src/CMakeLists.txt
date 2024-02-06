add_executable(draw draw_example.cpp)
target_link_directories(draw PUBLIC /usr/local/Cellar/fltk/1.3.9/lib/)
target_link_libraries(draw PRIVATE objparser mtr libfltk.a)