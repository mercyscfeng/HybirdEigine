//
// Created by Administrator on 2024/5/19.
//

#include "rendering.h"

namespace Hybrid{
  std::unique_ptr<Rendering> Rendering::_instance = nullptr;
  Rendering::Rendering() {

  }
  Rendering::~Rendering(){

  }
  Rendering Rendering::GetInstance() {
    if(!_instance){
      _instance.reset(new Rendering);
    }
    return *_instance;
  }
  void Rendering::Startup()
  {
      Context::GetInstance().StartUp();
      createWindow();
  }
  void Rendering::Destroy() {
      // 销毁窗口和终止 GLFW
      glfwDestroyWindow(window);
      glfwTerminate();
  }
  int Rendering::createWindow() {
      if (!glfwInit()) {
          return -1;
      }
      // 设置 GLFW 不使用 OpenGL
      glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

      //创建窗口
      GLFWwindow* window = glfwCreateWindow(800, 600, "Hybird Engine", nullptr, nullptr);
      if (!window) {
          // 创建窗口失败
          glfwTerminate();
          return -1;
      }
      // 初始化 Vulkan 表面
      VkSurfaceKHR surface;
      if (glfwCreateWindowSurface(Context::GetInstance().instance, window, nullptr, &surface) != VK_SUCCESS) {
          std::cerr << "Failed to create Vulkan surface" << std::endl;
          glfwDestroyWindow(window);
          glfwTerminate();
          return -1;
      }
      Context::GetInstance().surface = surface;
      // 初始化 Vulkan 设备、交换链等
      Context::GetInstance().createDevice();
      Context::GetInstance().createSwapChain();
      Context::GetInstance().createImageViews();
      Context::GetInstance().createRenderPass();
      Context::GetInstance().createGraphicsPipeline();
      Context::GetInstance().createFramebuffers();
      Context::GetInstance().createCommandPool();
      Context::GetInstance().createCommandBuffers();
      // 使用窗口上下文
      //glfwMakeContextCurrent(window);

      while (!glfwWindowShouldClose(window)) {
          if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
              glfwSetWindowShouldClose(window, true);
          }
          //vulkan 代码
          // 绘制帧
          Context::GetInstance().drawFrame();
          //glfwSwapBuffers(window);
          glfwPollEvents();
      }

      // 销毁窗口和终止 GLFW
      glfwDestroyWindow(window);
      glfwTerminate();
      return 0;
  }
}
