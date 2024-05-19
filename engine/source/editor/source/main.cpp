//
// Created by Administrator on 2024/5/16.
//
#include <iostream>

#include "runtime/engine.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    Runtime::Engine::GetInstance().StartUp();
    Runtime::Engine::GetInstance().CreateWindow();
    return 0;
}
