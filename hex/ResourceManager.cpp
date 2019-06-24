#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

std::map<std::string, Shader> ResourceManager::Shaders;
std::map<std::string, Texture2D> ResourceManager::Textures;
std::map<std::string, Cubemap> ResourceManager::Cubemaps;

Shader ResourceManager::LoadShader(std::string name, const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile) {
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

Shader& ResourceManager::GetShader(std::string name) {
    return Shaders[name];
}

void ResourceManager::Clear() {
    for (auto iter : Shaders) {
        glDeleteProgram(iter.second.ID);
    }
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile) {
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    
    try {
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;
        
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();
        vertexShaderFile.close();
        fragmentShaderFile.close();
        
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        
        if (gShaderFile != nullptr) {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e) {
        std::cout << "Error: Failed to read shader files" << std::endl;
    }
    
    const GLchar *vShaderCode = vertexCode.c_str();
    const GLchar *fShaderCode = fragmentCode.c_str();
    const GLchar *gShaderCode = geometryCode.c_str();
    
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}