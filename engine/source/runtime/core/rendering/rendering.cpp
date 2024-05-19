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
    if(!glfwInit()){
      return -1;
    }

    //创建窗口
    window = glfwCreateWindow(800,600,"Hybird Engine" , nullptr, nullptr);
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
    return 0;
  }
}
