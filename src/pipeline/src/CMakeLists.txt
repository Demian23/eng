add_library(pipe STATIC GraphicsPipeline.cpp GraphicsPipeline.h )
target_link_libraries(pipe PUBLIC options warnings ent)
