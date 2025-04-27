//
// Created by Administrator on 2025/4/26.
//

#include "VulkanRenderPass.h"
namespace Hybird{
    VulkanRenderPass::VulkanRenderPass()
    {

    }
    void VulkanRenderPass::CreateShaderModule(const std::string &shaderPath, const Hybird::ShaderType &type)
    {
        auto shaderCode = FileMgr::ReadFile(FileMgr::GetShaderPath(shaderPath));

    }

    vk::ShaderModule VulkanRenderPass::GetShader(const Hybird::ShaderType &type)
    {
        if(type == ShaderType::Vertex){
            return m_vertShader;
        }
        if(type == ShaderType::Fragment){
            return m_fragShader;
        }
        if(type == ShaderType::Compute){
            return m_compShader;
        }
        return nullptr;
    }
}

