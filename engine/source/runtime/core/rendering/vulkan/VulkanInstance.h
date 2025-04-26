//
// Created by Administrator on 2025/4/26.
//
#pragma once
#include "vulkan/vulkan.hpp"

/**
 * vulkan实例
 * 负责物理设备，逻辑设备，队列的创建和销毁
 * */
class VulkanInstance {
public:
    void Init();
    void Cleanup();
    VkDevice device;
    VkPhysicalDevice physicalDevice;
    VkQueue graphicsQueue;
};



