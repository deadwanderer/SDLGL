#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include "../include/glad.h"

#include "Shader.h"
#include "Texture2D.h"
#include "Cubemap.h"

class ResourceManager {
    public:
    static std::map<std::string, Shader> Shaders;
    static std::map<std::string, Texture2D> Textures;
    static std::map<std::string, Cubemap> Cubemaps;
    
    static Shader LoadShader(std::string name, const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
    static Shader &GetShader(std::string name);
    
    static Texture2D LoadTexture(std::string name, const GLchar *textureFile, GLboolean alpha = GL_FALSE);
    static Texture2D GetTexture(std::string name);
    
    static Cubemap LoadCubemap(std::string name);
    static Cubemap LoadCubemap(std::string name, const std::string cubemapName);
    static Cubemap LoadCubemap(std::string name, const std::vector<std::string> cubemapFiles);
    static Cubemap GetCubemap(std::string name);
    
    static void Clear();
    
    
    private:
    ResourceManager() {}
    static Shader loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile);
    static Texture2D loadTextureFromFile(const GLchar *textureFile, GLboolean alpha);
    static Cubemap loadCubemapFromFile(std::string cubemapName);
    static Cubemap loadCubemapFromVector(const std::vector<std::string> cubemapFiles);
};

#endif