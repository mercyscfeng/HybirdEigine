//
// Created by Administrator on 2024/5/16.
//

#pragma once

#include "vulkan/vulkan.h"
#include "GLFW/glfw3.h"
#include "core/context.hpp"
#include <memory>

namespace Runtime{
    class Engine {
    public:
        void StartUp();
        void Destory();
        int CreateWindow();
        static Engine GetInstance();
        ~Engine();
    private:
        Engine();
        static std::unique_ptr<Engine> _instance;
        GLFWwindow* window;
    };
}




