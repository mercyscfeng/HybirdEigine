#include "vulkan/vulkan.hpp"
#include <iostream>
#include <vector>
#include <string>

#include "../../../util/FileMgr.h"

namespace Hybird{
    class VulkanPassType
    {

    };
    enum ShaderType{
        Vertex,
        Fragment,
        Compute
    };

    class VulkanRenderPass {
    public:
        VulkanRenderPass();
        void CreateShaderModule(const std::string& shaderPath, const ShaderType& type);
    private:
        std::string m_name;
        vk::ShaderModule m_fragShader;
        vk::ShaderModule m_vertShader;
        vk::ShaderModule m_compShader;
        vk::ShaderModule GetShader(const ShaderType& type);
    };
}


