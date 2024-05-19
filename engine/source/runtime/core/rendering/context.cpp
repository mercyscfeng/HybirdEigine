#include "context.hpp"
#define APP_SHORT_NAME  "hybird_instance"

namespace Hybrid{
   std::unique_ptr<Context> Context::_instance = nullptr;
   void Context::StartUp() {
       createVkInstance();
       pickupPhysicalDevice();
   }

   void Context::Destory() {
       _instance->instance.destroy();
       _instance.reset();
   }

   Context& Context::GetInstance() {
       if(!_instance){
           _instance.reset(new Context);
       }
       return *_instance;
   }

   Context::Context() {
   }
   Context::~Context(){

   }
   void Context::createVkInstance() {
       std::cout << "create vkinstance " << std::endl;

       // initialize the VkInstanceCreateInfo structure
        vk::InstanceCreateInfo inst_info;
        vk::Result res =  vk::createInstance(&inst_info, nullptr,&instance);
        std::cout << make_error_code(res) << std::endl;
   }

   void Context::pickupPhysicalDevice() {
        auto devices = instance.enumeratePhysicalDevices();
        for(auto& device:devices){
//            auto feature = device.getFeatures();
            std::cout<< device.getProperties().deviceName << "---" << to_string(device.getProperties().deviceType)<< std::endl;
        }
       phyDevice = devices[0];
   }

   void Context::createDevice() {
//       vk::DeviceCreateInfo createInfo;
//       vk::DeviceQueueCreateInfo queueCreateInfo;
//       float  priorities = 1.0;
//       queueCreateInfo.setPQueuePriorities(&priorities)
//                      .setQueueCount(1)
//                      .setQueueFamilyIndex();
//       createInfo.setPQueueCreateInfos();
////       Command
//        device = phyDevice.createDevice(createInfo);
   }
}