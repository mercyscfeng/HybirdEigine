#pragma once
#include "vulkan/vulkan.hpp"
#include <memory>
#include <iostream>

namespace  Runtime {
	class Context final {
    public:
        void StartUp();
        void Destory();
        static Context& GetInstance();
        ~Context();
        vk::Instance  instance;
        vk::PhysicalDevice phyDevice;
        vk::Device device;
    private:
        Context();
        static std::unique_ptr<Context> _instance;
        void createVkInstance();
        void pickupPhysicalDevice();
        void createDevice();

	};
}