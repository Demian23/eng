add_library(deferred STATIC DeferredShaders.h DeferredShaders.cpp GBuffer.h)
target_link_libraries(deferred PUBLIC options warnings vec)
