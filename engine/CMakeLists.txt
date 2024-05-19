project(HybirdEngine)

set(ENGINE_ROOT_DIR "${PILOT_ROOT_DIR}/engine")
set(ENGINE_ASSET_DIR "/assets")
set(ENGINE_SCHEMA_DIR "/schema")
set(THIRD_PARTY "${CMAKE_CURRENT_SOURCE_DIR}/thirdParty")

if(APPLE)
    set(OS_TYPE "APPLE")
elseif(UNIX)
    set(OS_TYPE "LINUX")
elseif(WIN32)
    set(OS_TYPE "WINDOWS")
else()
    message(FATAL_ERROR "unkonwn os type")
endif()
message(STATUS "===>OS type: ${OS_TYPE}")


if (${OS_TYPE} STREQUAL "WINDOWS")
    set(BUILD_TARGET "Win32")
    if (CMAKE_CL_64)
        set(BUILD_TARGET "x64")
    endif()
    message(STATUS "Build target: ${BUILD_TARGET}")
    if (NOT DEFINED ${CMAKE_BUILD_TYPE})
        set(CMAKE_BUILD_TYPE "Release")
    endif()
    set(SYSTEM_LIBS "")
    #需要加该参数，不然VS会报错
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
else ()
    set(BUILD_TARGET "MacOS")
endif ()

set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lib/${BUILD_TARGET})

set(VULKAN_INCLUDE_DIR ${THIRD_PARTY}/vulkanSDK/include)
set(VULKAN_LIB ${THIRD_PARTY}/vulkanSDK/lib/${BUILD_TARGET}/vulkan-1.lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/source ${VULKAN_INCLUDE_DIR})


#add_subdirectory(shader)
add_subdirectory(source/runtime)
add_subdirectory(source/editor)
add_subdirectory(thirdParty)


#add_subdirectory(source/editor)
#add_subdirectory(source/test)

#set(CODEGEN_TARGET "PilotPreCompile")

#include(source/precompile/precompile.cmake)
#set_target_properties("${CODEGEN_TARGET}" PROPERTIES FOLDER "Engine" )
#
#add_dependencies(PilotRuntime "${CODEGEN_TARGET}")
