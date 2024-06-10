#ifndef BULLET_H
#define BULLET_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"

class Bullet {
public:
    Bullet(glm::vec3 position, glm::vec3 velocity, float speed, unsigned int textureID, Shader& shader) :
        position(position), velocity(velocity), speed(speed), textureID(textureID), shader(shader) , hasHit(false){
        Setup();
    }

    Bullet& operator=(const Bullet& other) {
        if (this != &other) {
            this->position = other.position;
            this->velocity = other.velocity;
            this->speed = other.speed;
            this->textureID = other.textureID;
            // ��������������Ա����...
        }
        return *this;
    }

    glm::vec3 position;
    bool hasHit; // �����ı�־λ

    void Draw() {
        shader.use();
        // ����ģ�;���Ӧ��λ�ñ任
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);

        shader.setMat4("model", model);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    void Update(float deltaTime) {
        
        position += velocity * speed * deltaTime;
    }

private:
    
    glm::vec3 velocity;
    float speed;
    unsigned int textureID;
    Shader& shader;
    unsigned int VAO;

    void Setup() {
        float vertices[] = {
            // Positions          // Texture Coords
            -0.075f, -0.075f, 0.0f,  0.0f, 0.0f,
             0.075f, -0.075f, 0.0f,  1.0f, 0.0f,
             0.075f,  0.075f, 0.0f,  1.0f, 1.0f,
             0.075f,  0.075f, 0.0f,  1.0f, 1.0f,
            -0.075f,  0.075f, 0.0f,  0.0f, 1.0f,
            -0.075f, -0.075f, 0.0f,  0.0f, 0.0f
        };
        
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // Position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Texture coordinate attribute
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif // BULLET_H
