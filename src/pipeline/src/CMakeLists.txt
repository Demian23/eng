add_library(pipe STATIC GraphicsPipeline.cpp GraphicsPipeline.h
        FlatShaderWithLambertColoring.h
        PhongShaderWithLambertColoring.h
)
target_link_libraries(pipe PUBLIC options warnings ent)
