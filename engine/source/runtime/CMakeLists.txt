set(TARGET_NAME HybirdRuntime)



include_directories(${CMAKE_CURRENT_SOURCE_DIR}  )

add_subdirectory(core)

add_library(${TARGET_NAME}
        $<TARGET_OBJECTS:core>
        engine.cpp
)



message("===> ${VULKAN_INCLUDE_DIR} ")

target_link_libraries(${TARGET_NAME} PUBLIC ${VULKAN_LIB} glfw)