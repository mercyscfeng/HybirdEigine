#pragma once
#include <iostream>
#include "../RGResource/RenderResource.h"
#include <utility>
#include <vector>
#include <functional>
#include <memory>

namespace Hybird{
    class RenderResource; //前向声明
    struct RenderPassContext {
        // 这里可以放本Pass需要的上下文，比如命令列表、资源句柄等
        // 例如:
        // CommandList* cmd;
        // std::unordered_map<std::string, void*> resourceHandles;
    };
    class RenderPass {
    public:
        using ExecuteRenderFunc = std::function<void(RenderPassContext&)>;
        [[nodiscard]] const std::string& GetName() const {return m_name;}
        explicit RenderPass(std::string name):m_name(std::move(name)){}
        void AddReadResource(RenderResource* res);
        void AddWriteResource(RenderResource* res);
        const std::vector<RenderResource*>& GetReadResources() const { return m_readResources; }
        const std::vector<RenderResource*>& GetWriteResources() const { return m_writeResources; }
        // 设置执行逻辑
        void SetExecuteFunc(ExecuteRenderFunc func) { m_executeFunc = std::move(func); }
        // 由RenderGraph调用
        void Execute(RenderPassContext& ctx) {
            if (m_executeFunc) m_executeFunc(ctx);
        }
    private:
        std::string m_name;
        std::vector<RenderResource*> m_readResources;
        std::vector<RenderResource*> m_writeResources;
        ExecuteRenderFunc m_executeFunc;
    };
}




