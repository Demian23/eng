add_library(pipe STATIC GraphicsPipeline.cpp GraphicsPipeline.h
        Shaders.h
        Shaders.cpp
)
target_link_libraries(pipe PUBLIC options warnings ent)
