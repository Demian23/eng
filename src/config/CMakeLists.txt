add_library(config INTERFACE)
configure_file(engConfig.h.in engConfig.h)
target_include_directories(config INTERFACE ${CMAKE_CURRENT_BINARY_DIR})