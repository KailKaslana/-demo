#pragma once
//
// shader.h
//
#ifndef SHADER_H
#define SHADER_H
// �� shader.h��
#include <glm/glm.hpp>
#include <string>
class Shader
{
public:
    // OpenGL ID
    unsigned int ID;
    // ��������ȡ��������ɫ��
    Shader(const char* vertexPath, const char* fragmentPath);
    // ʹ����ɫ��
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
