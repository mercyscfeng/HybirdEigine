//
// Created by Administrator on 2024/5/16.
//
#include <iostream>

#include "runtime/engine.h"
int main() {
    std::cout << "Hello, World!" << std::endl;
    Hybrid::Engine::GetInstance().StartUp();
    return 0;
}
