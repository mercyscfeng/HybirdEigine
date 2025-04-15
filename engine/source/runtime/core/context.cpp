#include "context.hpp"

namespace Runtime{
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
       // ��ȡ����������
       auto queueFamilyProperties = phyDevice.getQueueFamilyProperties();

       // ����֧��ͼ�β����Ķ�����
       for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
           if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
               graphicsQueueFamilyIndex = i;
               break;
           }
       }

       // ���ö��д�����Ϣ
       float queuePriority = 1.0f;
       vk::DeviceQueueCreateInfo queueCreateInfo;
       queueCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
       queueCreateInfo.queueCount = 1;
       queueCreateInfo.pQueuePriorities = &queuePriority;

       // �����豸������Ϣ
       vk::DeviceCreateInfo deviceCreateInfo;
       deviceCreateInfo.queueCreateInfoCount = 1;
       deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;

       // �����߼��豸
       device = phyDevice.createDevice(deviceCreateInfo);

       // ��ȡͼ�ζ���
       graphicsQueue = device.getQueue(graphicsQueueFamilyIndex, 0);
   }

   void Context::createSwapChain() {
       // ��ȡ��������
       vk::SurfaceCapabilitiesKHR surfaceCapabilities = phyDevice.getSurfaceCapabilitiesKHR(surface);

       // ��ȡ�����ʽ
       std::vector<vk::SurfaceFormatKHR> surfaceFormats = phyDevice.getSurfaceFormatsKHR(surface);
       vk::SurfaceFormatKHR surfaceFormat = surfaceFormats[0];

       // ����������
       vk::SwapchainCreateInfoKHR swapChainCreateInfo;
       swapChainCreateInfo.surface = surface;
       swapChainCreateInfo.minImageCount = surfaceCapabilities.minImageCount;
       swapChainCreateInfo.imageFormat = surfaceFormat.format;
       swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
       swapChainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
       swapChainCreateInfo.imageArrayLayers = 1;
       swapChainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
       swapChainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
       swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
       swapChainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
       swapChainCreateInfo.presentMode = vk::PresentModeKHR::eFifo;
       swapChainCreateInfo.clipped = VK_TRUE;

       swapChain = device.createSwapchainKHR(swapChainCreateInfo);

       // ��ȡ������ͼ��
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
       // ������ɫ������
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

       // ��������
       vk::PipelineVertexInputStateCreateInfo vertexInputInfo;

       // ����װ��
       vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
       inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
       inputAssembly.primitiveRestartEnable = VK_FALSE;

       // �ӿںͲü�
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

       // ��դ��
       vk::PipelineRasterizationStateCreateInfo rasterizer;
       rasterizer.depthClampEnable = VK_FALSE;
       rasterizer.rasterizerDiscardEnable = VK_FALSE;
       rasterizer.polygonMode = vk::PolygonMode::eFill;
       rasterizer.lineWidth = 1.0f;
       rasterizer.cullMode = vk::CullModeFlagBits::eBack;
       rasterizer.frontFace = vk::FrontFace::eClockwise;
       rasterizer.depthBiasEnable = VK_FALSE;

       // ���ز���
       vk::PipelineMultisampleStateCreateInfo multisampling;
       multisampling.sampleShadingEnable = VK_FALSE;
       multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;

       // ��ɫ���
       vk::PipelineColorBlendAttachmentState colorBlendAttachment;
       colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
       colorBlendAttachment.blendEnable = VK_FALSE;

       vk::PipelineColorBlendStateCreateInfo colorBlending;
       colorBlending.logicOpEnable = VK_FALSE;
       colorBlending.attachmentCount = 1;
       colorBlending.pAttachments = &colorBlendAttachment;

       // ���߲���
       vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
       pipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);

       // ����ͼ�ι���
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

       // ������ɫ��ģ��
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