#pragma once
#include <iostream>
#include <fstream>
#include <memory>

namespace Hybird{
    class FileMgr {
    public:
        static std::string GetRootPath()
        {
            return "D:\\HybirdEigen\\engine\\";
        }
        static std::string GetShaderPath(const std::string& shaderName){
            return GetRootPath().append("shader\\").append(shaderName);
        }
        static std::vector<char> ReadFile(const std::string& path)
        {
            std::ifstream file(path, std::ios::ate | std::ios::binary);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file: " + path);
            }
            long long fileSize = static_cast<long long >(file.tellg());
            std::vector<char> buffer(fileSize);
            file.seekg(0);
            file.read(buffer.data(), fileSize);
            file.close();

            return buffer;
        }
    };
}



