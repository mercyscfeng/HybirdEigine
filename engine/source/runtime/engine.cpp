//
// Created by Administrator on 2024/5/16.
//
#include "engine.h"

namespace Hybrid{
    std::unique_ptr<Engine> Engine::_instance = nullptr;
    void Engine::StartUp() {
        Rendering::GetInstance().Startup();
    }
    void Engine::Destory() {
        _instance.reset();
    }
    Engine Engine::GetInstance() {
        if(!_instance){
            _instance.reset(new Engine);
        }
        return *_instance;
    }
    Engine::~Engine(){
//        delete _instance.get();
    }
    Engine::Engine() {

    }

    void Engine::Tick() 
    {
        Rendering::GetInstance().createWindow();
    }
 
}