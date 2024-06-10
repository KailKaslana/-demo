#pragma once
#ifndef ENEMY_H
#define ENEMY_H
#include <windows.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader.h"
#include "bullet.h" // �����ӵ����ͷ�ļ�



class Enemy {
public:
    Enemy(glm::vec3 position, glm::vec3 size, unsigned int textureID, unsigned int bulletTextureID, Shader& shader) :
        position(position), size(size), textureID(textureID), bulletTextureID(bulletTextureID), shader(shader),
        bulletSpeed(1.2f), bulletInterval(0.075f), lastBulletTime(0.0f), health(99), damagetimes(0),
        curHP1(position, glm::vec3(0.3f, 0.3, 1.0f), "9", shader),
        curHP2(position, glm::vec3(0.3f, 0.3, 1.0f), "9", shader),
        maxHP(position, glm::vec3(0.9f, 0.3, 1.0f), "99", shader) {
        Setup();
    }
        glm::vec3 position;
        unsigned int textureID;
        glm::vec3 size;
        int health;             // Ѫ��
        int lasthealth;
        float bulletInterval;
        float bulletSpeed;
        Render curHP1;
        Render curHP2;
        Render maxHP;
    void Draw() {
        shader.use();

        // ����ģ�;���Ӧ��λ�úʹ�С�任
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, size);

        shader.setMat4("model", model);

        // ���Ƶ���
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        curHP1.Draw();
        curHP2.Draw();
        maxHP.Draw();
       
        // ���Ƶ��˵��ӵ�
        for (auto& bullet : bullets) {
            bullet.Draw();
        }
    }
    void TakeDamage(int damage) {
        if (damagetimes > 10) {             //�˺�ϵ����������˲ŵ�Ѫ
            health -= damage;
            if (health < 0) health = 0;
            damagetimes = 0;
        }
        damagetimes++;
    }
    void Heal(int amount) {
        health += amount;
        if (health > 99) health = 99;
    }
    int GetHealth() const {
        return health;
    }

    void Update(float deltaTime) {

        //��������λ��
        curHP1.setPosition(position + glm::vec3(-0.18f, 0.3f, 0.0f));
        curHP2.setPosition(position + glm::vec3(-0.1f, 0.3f, 0.0f));
        maxHP.setPosition(position + glm::vec3(0.07f, 0.3f, 0.0f));
        // �����ӵ�
        for (auto& bullet : bullets) {
            bullet.Update(deltaTime);
        }
        // �����ӵ�
        ShootBullets(deltaTime);

        //����Ѫ��
        if (health != lasthealth && health >= 0) {
            // ������ת��Ϊ�ַ���

            std::string str = std::to_string(health);

            std::string substr1;
            std::string substr2;
            // ��ȡ�ַ����������ַ�
            if (health >= 10) {
                substr1 = str.substr(0, 1);
                substr2 = str.substr(1, 1);
            }
            else {
                substr1 = "0";
                substr2 = str.substr(0, 1);
            }
            const char* str1 = substr1.c_str();    //  ȡ��һ���ַ�
            const char* str2 = substr2.c_str();    //  ȡ�ڶ����ַ�

            // ȷ���ַ����Կ��ַ���β
            char buffer1[2] = { str1[0], '\0' };
            char buffer2[2] = { str2[0], '\0' };

            curHP1.updateTexture(buffer1);
            curHP2.updateTexture(buffer2);

            lasthealth = health;
        }
    }
    std::vector<Bullet>& GetBullets() {
        return bullets;
    }



private:
    
    Shader& shader;
    unsigned int VAO;
    unsigned int bulletTextureID;
    float lastBulletTime;
    int damagetimes;
    std::vector<Bullet> bullets;

    

    void Setup() {
        float vertices[] = {
            // λ��          // ��������
            -0.2f, -0.2f, 0.0f,  0.0f, 0.0f,
             0.2f, -0.2f, 0.0f,  1.0f, 0.0f,
             0.2f,  0.2f, 0.0f,  1.0f, 1.0f,

             0.2f,  0.2f, 0.0f,  1.0f, 1.0f,
            -0.2f,  0.2f, 0.0f,  0.0f, 1.0f,
            -0.2f, -0.2f, 0.0f,  0.0f, 0.0f
        };

        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // λ������
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // ������������
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    int i = 0,j = 0,k = 1,m = 0,n = 0,strick = 1;

    void ShootBullets(float deltaTime) {
        float currentTime = glfwGetTime();
        if (currentTime - lastBulletTime > bulletInterval) {
            // �����������ӵ�
            if (strick == 1) {
                if (k == 1) {
                    if (j % 2 == 1) {
                        Bullet bullet(position, glm::vec3(-1.0f + i * 0.2, -1.0f, 0.0f), bulletSpeed, bulletTextureID, shader);
                        bullets.push_back(bullet);
                        if (i <= 8) { i++; }
                        else { i = 0; j++; }
                    }
                    else
                    {
                        Bullet bullet(position, glm::vec3(0.9f - i * 0.2, -1.0f, 0.0f), bulletSpeed, bulletTextureID, shader);
                        bullets.push_back(bullet);
                        if (i <= 8) { i++; }
                        else { i = 0; j++; }
                        if (j > 4) { j = 0;k = -k; }
                    }
                }
                else {
                    if (j % 3 == 1)
                        for (i = 0;i <= 8;i++)
                        {
                            Bullet bullet(position, glm::vec3(-1.0f + i * 0.22, -1.0f, 0.0f), bulletSpeed, bulletTextureID, shader);
                            bullets.push_back(bullet);
                        }
                    j++;
                    if (j > 20) { i = 0; j = 0;k = -k; }
                }
                strick = -strick;
            }
            else {
                if (n <= 50) { 
                    if (n>35 && n%2 == 1) {
                            Bullet bullet(glm::vec3(-1.5f, 0.0f, 0.0f), glm::vec3(1.0f, -2.0f, 0.0f), bulletSpeed, bulletTextureID, shader);
                            bullets.push_back(bullet);
                    }
                    if (n > 35 && n % 2 == 0) {
                        Bullet bullet(glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(-1.0f, -2.0f, 0.0f), bulletSpeed, bulletTextureID, shader);
                        bullets.push_back(bullet);
                    }
                    n++; }
                else {
                    for (int m = -1; m <= 2; ++m) {
                        glm::vec3 bulletPosition = position + glm::vec3(m * 0.2 - 0.1f, 0.0f, 0.0f);
                        if (m == -1 || m == 2)
                        {
                            bulletPosition += glm::vec3(0.0f, -0.1f, 0.0f);
                        }
                        Bullet bullet(bulletPosition, glm::vec3(0.0f, -0.6f, 0.0f), bulletSpeed, bulletTextureID, shader);
                        bullets.push_back(bullet);
                    }
                    n = 0;
                }
                strick = -strick;
            }
            
            
            // ���Ƴ����е��ӵ�
            for (auto it = bullets.begin(); it != bullets.end(); ) {
                if (it->hasHit) {
                    it = bullets.erase(it);  // ɾ�����е��ӵ��������µ�����
                    it->hasHit = false;
                }
                else {
                    ++it;  // ���û�����У�����������һ���ӵ�
                }
            }
            // ����Ƿ񳬹��������ƣ�������ɾ��ǰ����ӵ�
            if (bullets.size() > 160) {
                bullets.erase(bullets.begin(), bullets.begin() + 80);  
                
             }
            lastBulletTime = currentTime;
        }
    }
};

#endif // ENEMY_H
