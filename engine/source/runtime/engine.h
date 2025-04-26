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
        void StartEngine();
        void ShutDownEngine();
        void Destory();
        void Run();
        bool IsQuit() const {return m_is_quit;}

        Engine& operator=(const Engine&) = delete;
        static Engine GetInstance();
        ~Engine();
    protected:
        void RnederTick();
        void LogicalTick();
        bool m_is_quit;
    private:
        Engine();
        static std::unique_ptr<Engine> _instance;
    };
}




