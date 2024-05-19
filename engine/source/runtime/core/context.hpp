#include "vulkan/vulkan.h"
#include <memory>
#include <iostream>


namespace  hybirdEigine {
	class Context final {
    public:
        void StartUp();
        void Destory();
        static Context& GetInstance();
        ~Context();
        VkInstance  instance;
    private:
        Context();
        static std::unique_ptr<Context> _instance;
	};
}