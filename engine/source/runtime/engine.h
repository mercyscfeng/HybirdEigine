//
// Created by Administrator on 2024/5/16.
//

#pragma once

#include "runtime/core/rendering/rendering.h"
#include <memory>

namespace Hybrid{
    class Engine {
    public:
        void StartUp();
        void Destory();
        void Tick();
        static Engine GetInstance();
        ~Engine();
    private:
        Engine();
        static std::unique_ptr<Engine> _instance;
    };
}




