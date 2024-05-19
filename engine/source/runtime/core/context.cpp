#include "context.hpp"
#define APP_SHORT_NAME  "hybird_instance"

namespace hybirdEigine{
   std::unique_ptr<Context> Context::_instance = nullptr;
   void Context::StartUp() {
       _instance.reset(new Context);
   }

   void Context::Destory() {
       _instance.reset();
   }

   Context& Context::GetInstance() {
       return *_instance;
   }

   Context::Context() {

       std::cout << "create vkinstance " << std::endl;
       VkInstanceCreateInfo createInfo;

       // initialize the VkApplicationInfo structure
       VkApplicationInfo app_info = {};
       app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
       app_info.pNext = NULL;
       app_info.pApplicationName = APP_SHORT_NAME;
       app_info.applicationVersion = 1;
       app_info.pEngineName = APP_SHORT_NAME;
       app_info.engineVersion = 1;
       app_info.apiVersion = VK_API_VERSION_1_3;

        // initialize the VkInstanceCreateInfo structure
       VkInstanceCreateInfo inst_info = {};
       inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
       inst_info.pNext = NULL;
       inst_info.flags = 0;
       inst_info.pApplicationInfo = &app_info;
       inst_info.enabledExtensionCount = 0;
       inst_info.ppEnabledExtensionNames = NULL;
       inst_info.enabledLayerCount = 0;
       inst_info.ppEnabledLayerNames = NULL;



       VkResult res = vkCreateInstance(&createInfo,NULL,&instance);
       if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
           std::cout << "cannot find a compatible Vulkan ICD\n";
           exit(-1);
       } else if (res) {
           std::cout << "unknown error\n";
           exit(-1);
       }
       vkDestroyInstance(instance, NULL);
//       _instance = VKcreateInstance(createInfo);

   }
}