set(TARGET_NAME HybirdRuntime)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} )

add_subdirectory(core/rendering)

add_library(${TARGET_NAME}
        $<TARGET_OBJECTS:core>
        engine.cpp
        function/framework/component/camera/camera.cpp
        function/framework/component/camera/camera.h
        core/math/vector4.cpp
        core/math/vector4.h
        util/FileMgr.cpp
        util/FileMgr.h
)
target_link_libraries(${TARGET_NAME} PUBLIC glfw)



