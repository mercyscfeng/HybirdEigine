#include "context.hpp"
#include "GLFW/glfw3.h"
#include <memory>
namespace Hybrid{
  class Rendering {
  public:
    void Startup();
    void Destroy();
    static Rendering GetInstance();
    int createWindow();
    ~Rendering();
  private:
    Rendering();
    GLFWwindow* window;
    
    static std::unique_ptr<Rendering> _instance;
  };
}



