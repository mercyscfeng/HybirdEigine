#include "context.hpp"

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
       // Ӧ�ó�����Ϣ
       const std::vector<const char*> validationLayers = {
               "VK_LAYER_KHRONOS_validation"
       };
       vk::ApplicationInfo appInfo(
           "Hybrid Engine",          // Ӧ�ó�������
           VK_MAKE_VERSION(1, 0, 0), // Ӧ�ó���汾
           "No Engine",              // ��������
           VK_MAKE_VERSION(1, 0, 0), // ����汾
           VK_API_VERSION_1_0        // Vulkan API �汾
       );

       // ��ȡ GLFW ����� Vulkan ��չ
       uint32_t glfwExtensionCount = 0;
       const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

       vk::InstanceCreateInfo createInfo;
       createInfo.pApplicationInfo = &appInfo;
       createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
       createInfo.ppEnabledLayerNames = validationLayers.data();
       createInfo.enabledExtensionCount = glfwExtensionCount;
       createInfo.ppEnabledExtensionNames = glfwExtensions;

       // ��ӡ���õ���չ
       std::cout << "Enabled Vulkan extensions:" << std::endl;
       for (uint32_t i = 0; i < glfwExtensionCount; i++) {
           std::cout << "  " << glfwExtensions[i] << std::endl;
       }

       // ���� Vulkan ʵ��
       try {
           instance = vk::createInstance(createInfo);
           std::cout << "Vulkan instance created successfully!" << std::endl;
       }
       catch (const vk::SystemError& err) {
           throw std::runtime_error("Failed to create Vulkan instance: " + std::string(err.what()));
       }
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
        // 获取所有队列族属性
        auto queueFamilyProperties = phyDevice.getQueueFamilyProperties();

        // 找到同时支持 graphics 和 present 的队列族
        int graphicsPresentQueueFamilyIndex = -1;
        for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
            bool graphics = (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) != vk::QueueFlags();
            VkBool32 present = false;
            phyDevice.getSurfaceSupportKHR(i, surface, &present);

            std::cout << "Queue family " << i << ": graphics=" << graphics << ", present=" << present << std::endl;

            if (graphics && present) {
                graphicsPresentQueueFamilyIndex = i;
                break;
            }
        }

        if (graphicsPresentQueueFamilyIndex == -1) {
            throw std::runtime_error("No queue family supports both graphics and present!");
        }

        graphicsQueueFamilyIndex = graphicsPresentQueueFamilyIndex;

        // 设置队列创建信息
        float queuePriority = 1.0f;
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // 设备创建信息
        vk::DeviceCreateInfo deviceCreateInfo;
        const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = 1;
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;

        // 创建逻辑设备
        device = phyDevice.createDevice(deviceCreateInfo);
        presentQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
        // 获取队列
        graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
        //创建两组信号量
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            imageAvailableSemaphores.push_back(device.createSemaphore({}));
            renderFinishedSemaphores.push_back(device.createSemaphore({}));
            inFlightFences.push_back(device.createFence({ vk::FenceCreateFlagBits::eSignaled }));
        }
    }

   void Context::createSwapChain() {
       // ��������豸���߼��豸�Ƿ���Ч
       if (!phyDevice || !device) {
           throw std::runtime_error("Physical device or logical device is not initialized");
       }

       // �������Ƿ���Ч
       if (!surface) {
           throw std::runtime_error("Surface is not initialized");
       }

       // ��ȡ��������
       vk::SurfaceCapabilitiesKHR surfaceCapabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
       std::cout << "Surface capabilities: " << surfaceCapabilities.minImageCount << " images" << std::endl;

       // ��ȡ�����ʽ
       std::vector<vk::SurfaceFormatKHR> surfaceFormats = phyDevice.getSurfaceFormatsKHR(surface);
       if (surfaceFormats.empty()) {
           throw std::runtime_error("No supported surface formats found");
       }
       vk::SurfaceFormatKHR surfaceFormat = surfaceFormats[0];
       std::cout << "Surface format: " << vk::to_string(surfaceFormat.format) << std::endl;
       swapChainImageFormat = surfaceFormat.format;
       // ���ý�����ͼ��ߴ�
       swapChainExtent = surfaceCapabilities.currentExtent;
       if (swapChainExtent.width == 0 || swapChainExtent.height == 0) {
           swapChainExtent.width = 800;
           swapChainExtent.height = 600;
       }
       std::cout << "Swap chain extent: " << swapChainExtent.width << "x" << swapChainExtent.height << std::endl;

       // ���ý�����ͼ������
       uint32_t imageCount = surfaceCapabilities.minImageCount;
       if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
           imageCount = surfaceCapabilities.maxImageCount;
       }

       // ��ȡ֧�ֵĳ���ģʽ
       std::vector<vk::PresentModeKHR> presentModes = phyDevice.getSurfacePresentModesKHR(surface);
       vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo; // Ĭ��ģʽ
       for (const auto& mode : presentModes) {
           if (mode == vk::PresentModeKHR::eMailbox) {
               presentMode = mode; // ����ѡ�� Mailbox ģʽ
               break;
           }
       }

       // ����������
       vk::SwapchainCreateInfoKHR swapChainCreateInfo;
       swapChainCreateInfo.surface = surface;
       swapChainCreateInfo.minImageCount = imageCount;
       swapChainCreateInfo.imageFormat = surfaceFormat.format;
       swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
       swapChainCreateInfo.imageExtent = swapChainExtent;
       swapChainCreateInfo.imageArrayLayers = 1;
       swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
       swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
       swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
       swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
       swapChainCreateInfo.presentMode = presentMode;
       swapChainCreateInfo.clipped = VK_TRUE;

       swapChain = device.createSwapchainKHR(swapChainCreateInfo);
       std::cout << "Swapchain handle: " << swapChain << std::endl;
       swapChainImages = device.getSwapchainImagesKHR(swapChain);
   }

   void Context::createImageViews() {
       swapChainImageViews.resize(swapChainImages.size());

       for (size_t i = 0; i < swapChainImages.size(); i++) {
           vk::ImageViewCreateInfo createInfo;
           createInfo.image = swapChainImages[i];
           createInfo.viewType = vk::ImageViewType::e2D;
           createInfo.format = swapChainImageFormat;
           createInfo.components.r = vk::ComponentSwizzle::eIdentity;
           createInfo.components.g = vk::ComponentSwizzle::eIdentity;
           createInfo.components.b = vk::ComponentSwizzle::eIdentity;
           createInfo.components.a = vk::ComponentSwizzle::eIdentity;
           createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
           createInfo.subresourceRange.baseMipLevel = 0;
           createInfo.subresourceRange.levelCount = 1;
           createInfo.subresourceRange.baseArrayLayer = 0;
           createInfo.subresourceRange.layerCount = 1;

           swapChainImageViews[i] = device.createImageView(createInfo);
       }
   }

   void Context::createRenderPass() {
       vk::AttachmentDescription colorAttachment;
       colorAttachment.format = swapChainImageFormat;
       colorAttachment.samples = vk::SampleCountFlagBits::e1;
       colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
       colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
       colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
       colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
       colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
       colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

       vk::AttachmentReference colorAttachmentRef;
       colorAttachmentRef.attachment = 0;
       colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

       vk::SubpassDescription subpass;
       subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
       subpass.colorAttachmentCount = 1;
       subpass.pColorAttachments = &colorAttachmentRef;

       vk::RenderPassCreateInfo renderPassInfo;
       renderPassInfo.attachmentCount = 1;
       renderPassInfo.pAttachments = &colorAttachment;
       renderPassInfo.subpassCount = 1;
       renderPassInfo.pSubpasses = &subpass;

       renderPass = device.createRenderPass(renderPassInfo);
   }

   vk::ShaderModule Context::createShaderModule(const std::vector<char>& code) {
       vk::ShaderModuleCreateInfo createInfo{};
       createInfo.codeSize = code.size();
       createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

       vk::ShaderModule shaderModule;
       if (device.createShaderModule(&createInfo, nullptr, &shaderModule) != vk::Result::eSuccess) {
           throw std::runtime_error("Failed to create shader module");
       }
       return shaderModule;
   }

   std::vector<char>  Context::readFile(const std::string& filename) {
       std::cout <<"read file1 " << std::endl;
       std::ifstream file(filename, std::ios::ate | std::ios::binary);
       std::cout <<"read file2 " << std::endl;
       if (!file.is_open()) {
           throw std::runtime_error("Failed to open file: " + filename);
       }
       std::cout <<"read file3 " << std::endl;
       size_t fileSize = static_cast<size_t>(file.tellg());
       std::vector<char> buffer(fileSize);
       std::cout <<"read file4 " << std::endl;
       file.seekg(0);
       file.read(buffer.data(), fileSize);
       std::cout <<"read file5 " << std::endl;
       file.close();

       return buffer;
   }

   void Context::createGraphicsPipeline() {
       auto vertShaderCode = readFile("D:\\HybirdEigen\\engine\\shader\\vert.spv");
       auto fragShaderCode = readFile("D:\\HybirdEigen\\engine\\shader\\frag.spv");
       vk::ShaderModule fragShaderModule;
       vk::ShaderModule vertShaderModule;
       try{
           vertShaderModule = createShaderModule(vertShaderCode);
           fragShaderModule = createShaderModule(fragShaderCode);
       }catch(std::exception& e) {
           std::cout << e.what() << std::endl;
           return ;
       }
       vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
       vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
       vertShaderStageInfo.module = vertShaderModule;
       vertShaderStageInfo.pName = "main";

       vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
       fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
       fragShaderStageInfo.module = fragShaderModule;
       fragShaderStageInfo.pName = "main";

       vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

       vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

       vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
       inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
       inputAssembly.primitiveRestartEnable = VK_FALSE;

       vk::Viewport viewport;
       viewport.x = 0.0f;
       viewport.y = 0.0f;
       viewport.width = 800.0f;  // 例如 800
       viewport.height = 600.0f; // 例如 600
       viewport.minDepth = 0.0f;
       viewport.maxDepth = 1.0f;

       vk::Rect2D scissor;
       scissor.offset = vk::Offset2D{ 0, 0 };
       scissor.extent = swapChainExtent;

       vk::PipelineViewportStateCreateInfo viewportState;
       viewportState.viewportCount = 1;
       viewportState.pViewports = &viewport;
       viewportState.scissorCount = 1;
       viewportState.pScissors = &scissor;

//       // ��դ��
       vk::PipelineRasterizationStateCreateInfo rasterizer;
       rasterizer.depthClampEnable = VK_FALSE;
       rasterizer.rasterizerDiscardEnable = VK_FALSE;
       rasterizer.polygonMode = vk::PolygonMode::eFill;
       rasterizer.lineWidth = 1.0f;
       rasterizer.cullMode = vk::CullModeFlagBits::eBack;
       rasterizer.frontFace = vk::FrontFace::eClockwise;
       rasterizer.depthBiasEnable = VK_FALSE;
//
       vk::PipelineMultisampleStateCreateInfo multisampling;
       multisampling.sampleShadingEnable = VK_FALSE;
       multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
//
       vk::PipelineColorBlendAttachmentState colorBlendAttachment;
       colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
       colorBlendAttachment.blendEnable = VK_FALSE;

       vk::PipelineColorBlendStateCreateInfo colorBlending;
       colorBlending.logicOpEnable = VK_FALSE;
       colorBlending.attachmentCount = 1;
       colorBlending.pAttachments = &colorBlendAttachment;
//
//       // ���߲���
       vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
       pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
//
//       // ����ͼ�ι���
       vk::GraphicsPipelineCreateInfo pipelineInfo;
       pipelineInfo.stageCount = 2;
       pipelineInfo.pStages = shaderStages;
       pipelineInfo.pVertexInputState = &vertexInputInfo;
       pipelineInfo.pInputAssemblyState = &inputAssembly;
       pipelineInfo.pViewportState = &viewportState;
       pipelineInfo.pRasterizationState = &rasterizer;
       pipelineInfo.pMultisampleState = &multisampling;
       pipelineInfo.pColorBlendState = &colorBlending;
       pipelineInfo.layout = pipelineLayout;
       pipelineInfo.renderPass = renderPass;
       pipelineInfo.subpass = 0;
//
       graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;
//
//       // ������ɫ��ģ��
       device.destroyShaderModule(vertShaderModule);
       device.destroyShaderModule(fragShaderModule);
   }



   void Context::createFramebuffers() {
       swapChainFramebuffers.resize(swapChainImageViews.size());

       for (size_t i = 0; i < swapChainImageViews.size(); i++) {
           vk::ImageView attachments[] = { swapChainImageViews[i] };

           vk::FramebufferCreateInfo framebufferInfo;
           framebufferInfo.renderPass = renderPass;
           framebufferInfo.attachmentCount = 1;
           framebufferInfo.pAttachments = attachments;
           framebufferInfo.width = 800;
           framebufferInfo.height = 600;
           framebufferInfo.layers = 1;

           swapChainFramebuffers[i] = device.createFramebuffer(framebufferInfo);
       }
   }

   void Context::createCommandPool() {
       vk::CommandPoolCreateInfo poolInfo;
       poolInfo.queueFamilyIndex = graphicsQueueFamilyIndex;

       commandPool = device.createCommandPool(poolInfo);
   }

   void Context::createCommandBuffers() {
       commandBuffers.resize(swapChainFramebuffers.size());

       vk::CommandBufferAllocateInfo allocInfo;
       allocInfo.commandPool = commandPool;
       allocInfo.level = vk::CommandBufferLevel::ePrimary;
       allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

       commandBuffers = device.allocateCommandBuffers(allocInfo);

       for (size_t i = 0; i < commandBuffers.size(); i++) {
           vk::CommandBufferBeginInfo beginInfo;

           commandBuffers[i].begin(beginInfo);

           vk::RenderPassBeginInfo renderPassInfo;
           renderPassInfo.renderPass = renderPass;
           renderPassInfo.framebuffer = swapChainFramebuffers[i];
           renderPassInfo.renderArea.offset = vk::Offset2D{ 0, 0 };
           renderPassInfo.renderArea.extent = swapChainExtent;

           vk::ClearValue clearColor = vk::ClearColorValue(std::array<float, 4>{0.0f, 0.0f, 0.0f, 1.0f});
           renderPassInfo.clearValueCount = 1;
           renderPassInfo.pClearValues = &clearColor;

           commandBuffers[i].beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
           commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, graphicsPipeline);
           commandBuffers[i].draw(3, 1, 0, 0);
           commandBuffers[i].endRenderPass();

           commandBuffers[i].end();
       }
   }

   void Context::drawFrame() {
       device.waitForFences(inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
       device.resetFences(inFlightFences[currentFrame]);

       auto acquireResult = device.acquireNextImageKHR(swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], nullptr);
       if (acquireResult.result == vk::Result::eErrorOutOfDateKHR) {
           // 需要重建 swapchain
           return;
       }
       uint32_t imageIndex = acquireResult.value;

       vk::SubmitInfo submitInfo;
       vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
       submitInfo.waitSemaphoreCount = 1;
       submitInfo.pWaitSemaphores = &imageAvailableSemaphores[currentFrame];
       submitInfo.pWaitDstStageMask = waitStages;
       submitInfo.commandBufferCount = 1;
       submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
       submitInfo.signalSemaphoreCount = 1;
       submitInfo.pSignalSemaphores = &renderFinishedSemaphores[currentFrame];

       graphicsQueue.submit(submitInfo, inFlightFences[currentFrame]);

       vk::PresentInfoKHR presentInfo;
       presentInfo.waitSemaphoreCount = 1;
       presentInfo.pWaitSemaphores = &renderFinishedSemaphores[currentFrame];
       presentInfo.swapchainCount = 1;
       presentInfo.pSwapchains = &swapChain;
       presentInfo.pImageIndices = &imageIndex;
       presentQueue.presentKHR(presentInfo);
       currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
   }

    void Context::cleanup() {
        device.waitIdle();
        for (auto framebuffer : swapChainFramebuffers) {
            device.destroyFramebuffer(framebuffer);
        }
        // 销毁信号量和fence
        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            device.destroySemaphore(imageAvailableSemaphores[i]);
            device.destroySemaphore(renderFinishedSemaphores[i]);
            device.destroyFence(inFlightFences[i]);
        }

        device.destroyPipeline(graphicsPipeline);
        device.destroyPipelineLayout(pipelineLayout);
        device.destroyRenderPass(renderPass);

        for (auto imageView : swapChainImageViews) {
            device.destroyImageView(imageView);
        }

        device.destroySwapchainKHR(swapChain);

        // 先释放 command buffer
        device.freeCommandBuffers(commandPool, commandBuffers);

        // 再销毁 command pool
        device.destroyCommandPool(commandPool);

        // 最后销毁 device
        device.destroy();
    }
}