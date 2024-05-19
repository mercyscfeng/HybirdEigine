//
// Created by Administrator on 2024/5/16.
//

#include "engine.h"

namespace Runtime{
    std::unique_ptr<Engine> Engine::_instance = nullptr;
    void Engine::StartUp() {
        Context::GetInstance().StartUp();
    }
    void Engine::Destory() {
        _instance.reset();
    }
    Engine Engine::GetInstance() {
        if(!_instance){
            _instance.reset(new Engine);
        }
        return *_instance;
    }
    Engine::~Engine(){
//        delete _instance.get();
    }
    Engine::Engine() {

    }
    int Engine::CreateWindow() {
        if(!glfwInit()){
            return -1;
        }

        //创建窗口
        GLFWwindow* window = glfwCreateWindow(800,600,"Hybird Engine" , nullptr, nullptr);
        if(!window){
            // 创建窗口失败
            glfwTerminate();
            return -1;
        }

        // 使用窗口上下文
        glfwMakeContextCurrent(window);

        while(!glfwWindowShouldClose(window)){
            if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS){
                glfwSetWindowShouldClose(window, true);
            }
            //vulkan 代码

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // 销毁窗口和终止 GLFW
        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
}