#pragma once
#include <iostream>
#include "runtime/core/rendering/renderGraph/RGResource/RenderResource.h"
#include <vector>
#include <functional>
namespace Hybird{
    class RenderPass {
        std::string name;
        std::vector<RenderResource*> readResource;
        std::vector<RenderResource*> writeResource;
        std::function<void> Execute();
    };
}




