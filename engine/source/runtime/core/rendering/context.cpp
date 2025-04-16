#include "context.hpp"

namespace Hybrid{
   std::unique_ptr<Context> Context::_instance = nullptr;
   void Context::StartUp() {
       createVkInstance();
       pickupPhysicalDevice();
       createDevice();
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
       // 应用程序信息
       vk::ApplicationInfo appInfo(
           "Hybrid Engine",          // 应用程序名称
           VK_MAKE_VERSION(1, 0, 0), // 应用程序版本
           "No Engine",              // 引擎名称
           VK_MAKE_VERSION(1, 0, 0), // 引擎版本
           VK_API_VERSION_1_0        // Vulkan API 版本
       );

       // 获取 GLFW 所需的 Vulkan 扩展
       uint32_t glfwExtensionCount = 0;
       const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

       // 实例创建信息
       vk::InstanceCreateInfo createInfo(
           vk::InstanceCreateFlags(), // 标志
           &appInfo,                  // 应用程序信息
           0, nullptr,                // 启用的验证层（暂时为空）
           glfwExtensionCount,        // 启用的扩展数量
           glfwExtensions             // 启用的扩展名称
       );

       // 打印启用的扩展
       std::cout << "Enabled Vulkan extensions:" << std::endl;
       for (uint32_t i = 0; i < glfwExtensionCount; i++) {
           std::cout << "  " << glfwExtensions[i] << std::endl;
       }

       // 创建 Vulkan 实例
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
       // 获取队列族属性
       auto queueFamilyProperties = phyDevice.getQueueFamilyProperties();

       // 查找支持图形操作的队列族
       for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
           if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
               graphicsQueueFamilyIndex = i;
               break;
           }
       }

       // 设置队列创建信息
       float queuePriority = 1.0f;
       vk::DeviceQueueCreateInfo queueCreateInfo;
       queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
       queueCreateInfo.queueCount = 1;
       queueCreateInfo.pQueuePriorities = &queuePriority;

       // 设置设备创建信息
       vk::DeviceCreateInfo deviceCreateInfo;
       deviceCreateInfo.queueCreateInfoCount = 1;
       deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

       // 创建逻辑设备
       device = phyDevice.createDevice(deviceCreateInfo);

       // 获取图形队列
       graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
   }

   void Context::createSwapChain() {
       // 检查物理设备和逻辑设备是否有效
       if (!phyDevice || !device) {
           throw std::runtime_error("Physical device or logical device is not initialized");
       }

       // 检查表面是否有效
       if (!surface) {
           throw std::runtime_error("Surface is not initialized");
       }

       // 获取表面能力
       vk::SurfaceCapabilitiesKHR surfaceCapabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);
       std::cout << "Surface capabilities: " << surfaceCapabilities.minImageCount << " images" << std::endl;

       // 获取表面格式
       std::vector<vk::SurfaceFormatKHR> surfaceFormats = phyDevice.getSurfaceFormatsKHR(surface);
       if (surfaceFormats.empty()) {
           throw std::runtime_error("No supported surface formats found");
       }
       vk::SurfaceFormatKHR surfaceFormat = surfaceFormats[0];
       std::cout << "Surface format: " << vk::to_string(surfaceFormat.format) << std::endl;

       // 设置交换链图像尺寸
       vk::Extent2D swapChainExtent = surfaceCapabilities.currentExtent;
       if (swapChainExtent.width == 0 || swapChainExtent.height == 0) {
           swapChainExtent.width = 800;  // 窗口宽度
           swapChainExtent.height = 600; // 窗口高度
       }
       std::cout << "Swap chain extent: " << swapChainExtent.width << "x" << swapChainExtent.height << std::endl;

       // 设置交换链图像数量
       uint32_t imageCount = surfaceCapabilities.minImageCount;
       if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
           imageCount = surfaceCapabilities.maxImageCount;
       }

       // 获取支持的呈现模式
       std::vector<vk::PresentModeKHR> presentModes = phyDevice.getSurfacePresentModesKHR(surface);
       vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo; // 默认模式
       for (const auto& mode : presentModes) {
           if (mode == vk::PresentModeKHR::eMailbox) {
               presentMode = mode; // 优先选择 Mailbox 模式
               break;
           }
       }

       // 创建交换链
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

       swapChain = vk::SwapchainKHR(); // 显式初始化为空
       vk::Result result = device.createSwapchainKHR(&swapChainCreateInfo, nullptr, &swapChain);
       std::cout << swapChain << vk::to_string(result)  << std::endl;
       if (result != vk::Result::eSuccess) {
           throw std::runtime_error("Failed to create swap chain: " + vk::to_string(result));
       }
       // 检查交换链是否有效
       //if (!swapChain) {
       //    throw std::runtime_error("Swap chain is not initialized");
       //}

       // 获取交换链图像
       /*try {
           vk::Result result = device.getSwapchainImagesKHR(swapChain, &imageCount, swapChainImages);
           if (result != vk::Result::eSuccess) {
               throw std::runtime_error("Failed to get swap chain images: " + vk::to_string(result));
           }
           std::cout << "Retrieved " << swapChainImages.size() << " swap chain images" << std::endl;
       }
       catch (const vk::SystemError& err) {
           throw std::runtime_error("Failed to retrieve swap chain images: " + std::string(err.what()));
       }*/
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
       std::ifstream file(filename, std::ios::ate | std::ios::binary);

       if (!file.is_open()) {
           throw std::runtime_error("Failed to open file: " + filename);
       }

       size_t fileSize = static_cast<size_t>(file.tellg());
       std::vector<char> buffer(fileSize);

       file.seekg(0);
       file.read(buffer.data(), fileSize);

       file.close();

       return buffer;
   }

   void Context::createGraphicsPipeline() {
       // 加载着色器代码
       auto vertShaderCode = readFile("../../../shader/vert.spv");
       auto fragShaderCode = readFile("../../../shader/frag.spv");

       vk::ShaderModule vertShaderModule = createShaderModule(vertShaderCode);
       vk::ShaderModule fragShaderModule = createShaderModule(fragShaderCode);

       vk::PipelineShaderStageCreateInfo vertShaderStageInfo;
       vertShaderStageInfo.stage = vk::ShaderStageFlagBits::eVertex;
       vertShaderStageInfo.module = vertShaderModule;
       vertShaderStageInfo.pName = "main";

       vk::PipelineShaderStageCreateInfo fragShaderStageInfo;
       fragShaderStageInfo.stage = vk::ShaderStageFlagBits::eFragment;
       fragShaderStageInfo.module = fragShaderModule;
       fragShaderStageInfo.pName = "main";

       vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

       // 顶点输入
       vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

       // 输入装配
       vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
       inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
       inputAssembly.primitiveRestartEnable = VK_FALSE;

       // 视口和裁剪
       vk::Viewport viewport;
       viewport.x = 0.0f;
       viewport.y = 0.0f;
       viewport.width = (float)swapChainExtent.width;
       viewport.height = (float)swapChainExtent.height;
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

       // 光栅化
       vk::PipelineRasterizationStateCreateInfo rasterizer;
       rasterizer.depthClampEnable = VK_FALSE;
       rasterizer.rasterizerDiscardEnable = VK_FALSE;
       rasterizer.polygonMode = vk::PolygonMode::eFill;
       rasterizer.lineWidth = 1.0f;
       rasterizer.cullMode = vk::CullModeFlagBits::eBack;
       rasterizer.frontFace = vk::FrontFace::eClockwise;
       rasterizer.depthBiasEnable = VK_FALSE;

       // 多重采样
       vk::PipelineMultisampleStateCreateInfo multisampling;
       multisampling.sampleShadingEnable = VK_FALSE;
       multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

       // 颜色混合
       vk::PipelineColorBlendAttachmentState colorBlendAttachment;
       colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
       colorBlendAttachment.blendEnable = VK_FALSE;

       vk::PipelineColorBlendStateCreateInfo colorBlending;
       colorBlending.logicOpEnable = VK_FALSE;
       colorBlending.attachmentCount = 1;
       colorBlending.pAttachments = &colorBlendAttachment;

       // 管线布局
       vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
       pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

       // 创建图形管线
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

       graphicsPipeline = device.createGraphicsPipeline(nullptr, pipelineInfo).value;

       // 销毁着色器模块
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
           framebufferInfo.width = swapChainExtent.width;
           framebufferInfo.height = swapChainExtent.height;
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
       vk::SemaphoreCreateInfo semaphoreInfo;
       vk::FenceCreateInfo fenceInfo;
       vk::Semaphore imageAvailableSemaphore = device.createSemaphore(semaphoreInfo);
       vk::Semaphore renderFinishedSemaphore = device.createSemaphore(semaphoreInfo);
       vk::Fence inFlightFence = device.createFence(fenceInfo);

       uint32_t imageIndex = device.acquireNextImageKHR(swapChain, UINT64_MAX, imageAvailableSemaphore, nullptr).value;

       vk::SubmitInfo submitInfo;
       vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
       submitInfo.waitSemaphoreCount = 1;
       submitInfo.pWaitSemaphores = &imageAvailableSemaphore;
       submitInfo.pWaitDstStageMask = waitStages;
       submitInfo.commandBufferCount = 1;
       submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
       submitInfo.signalSemaphoreCount = 1;
       submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

       graphicsQueue.submit(submitInfo, inFlightFence);

       vk::PresentInfoKHR presentInfo;
       presentInfo.waitSemaphoreCount = 1;
       presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
       presentInfo.swapchainCount = 1;
       presentInfo.pSwapchains = &swapChain;
       presentInfo.pImageIndices = &imageIndex;

       presentQueue.presentKHR(presentInfo);

       device.waitForFences(inFlightFence, VK_TRUE, UINT64_MAX);

       device.destroySemaphore(imageAvailableSemaphore);
       device.destroySemaphore(renderFinishedSemaphore);
       device.destroyFence(inFlightFence);
   }

   void Context::cleanup() {
       for (auto framebuffer : swapChainFramebuffers) {
           device.destroyFramebuffer(framebuffer);
       }

       device.destroyPipeline(graphicsPipeline);
       device.destroyPipelineLayout(pipelineLayout);
       device.destroyRenderPass(renderPass);

       for (auto imageView : swapChainImageViews) {
           device.destroyImageView(imageView);
       }

       device.destroySwapchainKHR(swapChain);
       device.destroy();
   }
}