#pragma once
#ifndef RENDER_H
#define RENDER_H

#include <windows.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string.h>
#include "shader.h"



class Render {
public:
    Render(glm::vec3 position, glm::vec3 size, char const * inst, Shader& shader) :
        position(position), size(size), textureID(textureID), shader(shader)
    {
        Setup();
        updateTexture(inst);
    }

    void Draw() {
        shader.use();

        // 创建模型矩阵并应用位置和大小变换
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, size);

        shader.setMat4("model", model);

        // 绘制文字
        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

    }
    void setPosition(glm::vec3 newPosition) {
        position = newPosition;
    }

    // 更新纹理图片
    void updateTexture(const char* str) {

        // 计算字符串长度
        size_t len = strlen(str);
        // 分配足够的内存来存储字符串，包括终止空字符
        char* ins = new char[len + 1];
        // 复制字符串到新分配的内存中
        strcpy_s(ins, len + 1, str);
        if (strcmp(ins, "0") == 0) {
            texture = loadTexture("res/char/0.png");
        }
        else if (strcmp(ins, "9") == 0) {
            texture = loadTexture("res/char/9.png");
        }
        else if (strcmp(ins, "8") == 0) {
            texture = loadTexture("res/char/8.png");
        }
        else if (strcmp(ins, "7") == 0) {
            texture = loadTexture("res/char/7.png");
        }
        else if (strcmp(ins, "6") == 0) {
            texture = loadTexture("res/char/6.png");
        }
        else if (strcmp(ins, "5") == 0) {
            texture = loadTexture("res/char/5.png");
        }
        else if (strcmp(ins, "4") == 0) {
            texture = loadTexture("res/char/4.png");
        }
        else if (strcmp(ins, "3") == 0) {
            texture = loadTexture("res/char/3.png");
        }
        else if (strcmp(ins, "2") == 0) {
            texture = loadTexture("res/char/2.png");
        }
        else if (strcmp(ins, "1") == 0) {
            texture = loadTexture("res/char/1.png");
        }
        else if (strcmp(ins, "99") == 0) {
            texture = loadTexture("res/char/99.png");
        }
        // 释放内存
        delete[] ins;
    }


private:
    glm::vec3 position;
    glm::vec3 size;
    unsigned int textureID;
    Shader& shader;
    unsigned int VAO;
    unsigned int texture;

    void Setup() {
        float vertices[] = {
            // 位置          // 纹理坐标
            -0.15f, -0.2f, 0.0f,  0.0f, 1.0f, // 左下角
             0.15f, -0.2f, 0.0f,  1.0f, 1.0f, // 右下角
             0.15f,  0.2f, 0.0f,  1.0f, 0.0f, // 右上角

             0.15f,  0.2f, 0.0f,  1.0f, 0.0f, // 右上角
            -0.15f,  0.2f, 0.0f,  0.0f, 0.0f, // 左上角
            -0.15f, -0.2f, 0.0f,  0.0f, 1.0f  // 左下角
        };



        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 纹理坐标属性
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    unsigned int loadTexture(char const* path)
    {
        unsigned int texture;
        glGenTextures(1, &texture);

        glBindTexture(GL_TEXTURE_2D, texture); // 先绑定纹理对象

        int width, height, nrComponents;
        unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            // 设置纹理参数
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // 使用 GL_CLAMP_TO_EDGE
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            // 加载纹理数据
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return texture;
    }

};


#endif // RENDER_H