file(GLOB CORE_SOURCES *.cpp)

add_library(core OBJECT ${CORE_SOURCES}
        vulkan/VulkanInstance.cpp
        vulkan/VulkanInstance.h
        vulkan/VulkanSwapchain.cpp
        vulkan/VulkanSwapchain.h
        vulkan/VulkanRenderPass.cpp
        vulkan/VulkanRenderPass.h
        vulkan/VulkanPipeline.cpp
        vulkan/VulkanPipeline.h
        vulkan/VulkanCommand.cpp
        vulkan/VulkanCommand.h
        vulkan/VulkanSync.cpp
        vulkan/VulkanSync.h
        renderGraph/RenderGraph.cpp
        renderGraph/RenderGraph.h
        renderGraph/RenderPass/RenderPass.cpp
        renderGraph/RenderPass/RenderPass.h
        renderGraph/RGResource/RenderResource.cpp
        renderGraph/RGResource/RenderResource.h
        renderGraph/RGResource/RenderResourceManager.cpp
        renderGraph/RGResource/RenderResourceManager.h)

target_link_libraries(core PUBLIC glfw )
