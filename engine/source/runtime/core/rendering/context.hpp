#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>

namespace  Hybrid {
	class Context final {
    public:
        void StartUp();
        void Destory();
        struct QueueFamilyIndices final {
          std::optional<uint32_t >graphicsQueue;
        };
        static Context& GetInstance();
        ~Context();
        vk::Instance  instance;
        vk::PhysicalDevice phyDevice;
        vk::Device device;
        QueueFamilyIndices queueFamilyIndices;
    private:
        Context();
        static std::unique_ptr<Context> _instance;
        void createVkInstance();
        void pickupPhysicalDevice();
        void createDevice();
        void queryQueueFamilyIndices();
    };
}