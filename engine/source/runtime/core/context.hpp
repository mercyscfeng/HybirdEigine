#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>
#include <vector>
#include <fstream>
#include <stdexcept>

namespace  Runtime {
	class Context final {
    public:
        void StartUp();
        void Destory();
        static Context& GetInstance();
        ~Context();
        vk::Instance instance;
        vk::SurfaceKHR surface;
        void createVkInstance();
        void pickupPhysicalDevice();
        void createDevice();
        void createSwapChain();
        void createImageViews();
        void createRenderPass();
        void createGraphicsPipeline();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void drawFrame();
        void cleanup();
    private:
        Context();
        static std::unique_ptr<Context> _instance;
        
        vk::ShaderModule createShaderModule(const std::vector<char>& code);
        std::vector<char> readFile(const std::string& filename);
 
        vk::PhysicalDevice phyDevice;
        vk::Device device;
        vk::Queue graphicsQueue;
        vk::Queue presentQueue;
        
        vk::SwapchainKHR swapChain;
        std::vector<vk::Image> swapChainImages;
        vk::Format swapChainImageFormat;
        vk::Extent2D swapChainExtent;
        std::vector<vk::ImageView> swapChainImageViews;
        vk::RenderPass renderPass;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline graphicsPipeline;
        std::vector<vk::Framebuffer> swapChainFramebuffers;
        vk::CommandPool commandPool;
        std::vector<vk::CommandBuffer> commandBuffers;
        uint32_t graphicsQueueFamilyIndex = 0;
	};
}