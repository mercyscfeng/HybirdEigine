#include "context.hpp"
#include "GLFW/glfw3.h"
#include <memory>
namespace Hybrid{
  class Rendering {
  public:
    void Startup();
    void Destroy();
    static Rendering GetInstance();
    ~Rendering();
  private:
    Rendering();
    GLFWwindow* window;
    int createWindow();
    static std::unique_ptr<Rendering> _instance;
  };
}



