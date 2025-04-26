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
  const char* vkResultToString(VkResult result) {
      switch (result) {
      case VK_SUCCESS: return "VK_SUCCESS";
      case VK_NOT_READY: return "VK_NOT_READY";
      case VK_TIMEOUT: return "VK_TIMEOUT";
      case VK_EVENT_SET: return "VK_EVENT_SET";
      case VK_EVENT_RESET: return "VK_EVENT_RESET";
      case VK_INCOMPLETE: return "VK_INCOMPLETE";
      case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
      case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
      case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
      case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
      case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
      case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
      case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
      case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
      case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
      case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
      case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
      case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
      case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
      case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
      case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
      case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
      case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
      case VK_ERROR_INVALID_SHADER_NV: return "VK_ERROR_INVALID_SHADER_NV";
      default: return "Unknown VkResult";
      }
  }
  Rendering Rendering::GetInstance() {
    if(!_instance){
      _instance.reset(new Rendering);
    }
    return *_instance;
  }
  void Rendering::Startup()
  {
      
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
      std::cout << "create window " << std::endl;
      Context::GetInstance().StartUp();
      // 检查 Vulkan 支持
      if (!glfwVulkanSupported()) {
          std::cerr << "Vulkan is not supported" << std::endl;
          glfwTerminate();
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
      VkResult result = glfwCreateWindowSurface(Context::GetInstance().instance, window, nullptr, &surface);
      if (result != VK_SUCCESS) {
          std::cerr << "Failed to create Vulkan surface: " << vkResultToString(result) << std::endl;
          glfwDestroyWindow(window);
          glfwTerminate();
          return -1;
      }
      Context::GetInstance().surface = vk::SurfaceKHR(surface);
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
      glfwMakeContextCurrent(window);

      while (!glfwWindowShouldClose(window)) {
          if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
              glfwSetWindowShouldClose(window, true);
          }
          //vulkan 代码
          // 绘制帧
          Context::GetInstance().drawFrame();
          glfwSwapBuffers(window);
          glfwPollEvents();
      }

      // 销毁窗口和终止 GLFW
      Context::GetInstance().cleanup();
      glfwDestroyWindow(window);
      glfwTerminate();
      return 0;
  }

    
  }

