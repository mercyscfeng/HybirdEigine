set(TARGET_NAME HybirdRuntime)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory(core/rendering)

add_library(${TARGET_NAME}
        $<TARGET_OBJECTS:core>
        engine.cpp
)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)



