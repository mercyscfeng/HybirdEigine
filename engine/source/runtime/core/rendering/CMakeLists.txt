file(GLOB CORE_SOURCES *.cpp)

add_library(core OBJECT ${CORE_SOURCES})

target_link_libraries(core PUBLIC glfw )
