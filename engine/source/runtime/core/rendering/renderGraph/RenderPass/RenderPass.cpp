#include "RenderPass.h"

namespace Hybird{
    void RenderPass::AddReadResource(Hybird::RenderResource *res) {
        if(res== nullptr) return;
        m_readResources.push_back(res);
    }

    void RenderPass::AddWriteResource(Hybird::RenderResource *res) {
        if(res== nullptr) return;
        m_writeResources.push_back(res);
    }
}