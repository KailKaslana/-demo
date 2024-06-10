#pragma once
//
// shader.h
//
#ifndef SHADER_H
#define SHADER_H
// 在 shader.h中
#include <glm/glm.hpp>
#include <string>
class Shader
{
public:
    // OpenGL ID
    unsigned int ID;
    // 构造器读取并构建着色器
    Shader(const char* vertexPath, const char* fragmentPath);
    // 使用着色器
    void use();
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;
    void setVec3(const std::string& name, glm::vec3 value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec2(const std::string& name, float x, float y) const;
};
#endif
