#ifndef CHARACTER_H
#define CHARACTER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL/SDL.h>
#include <string>
#include "shader.h"
#include "bullet.h"
#include "render.h"

class Character {
public:
    Character(glm::vec3 position, glm::vec3 size, unsigned int textureID, unsigned int bulletTextureID, Shader& shader) :
        position(position), size(size), textureID(textureID), bulletTextureID(bulletTextureID), shader(shader),
        bulletSpeed(5.0f), bulletInterval(0.25f), lastBulletTime(0.0f) , health(20),
        curHP1(position, glm::vec3(0.3f, 0.3, 1.0f), "9", shader),
        curHP2(position, glm::vec3(0.3f, 0.3, 1.0f), "9", shader) ,
        maxHP(position, glm::vec3(0.9f, 0.3, 1.0f), "99", shader) {
        Setup();
    }

    glm::vec3 position;
    glm::vec3 size;
    float speed = 0.25;     //移速
    int health;             // 血量
    void Draw() {
        shader.use();

        // 创建模型矩阵并应用位置和大小变换
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::scale(model, size);

        shader.setMat4("model", model);

        // 绘制角色
        glBindTexture(GL_TEXTURE_2D, textureID);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        curHP1.Draw();
        curHP2.Draw();
        maxHP.Draw();
        // 绘制子弹
        for (auto& bullet : bullets) {
            bullet.Draw();
        }
    }

    void TakeDamage(int damage) {
        health -= damage*0.2;
        if (health < 0) health = 0;
    }
    void Heal(int amount) {
        health += amount;
        if (health > 99) health = 99;
    }
    int GetHealth() const {
        return health;
    }
    std::vector<Bullet>& GetBullets() {
        return bullets;
    }

    void Update(float deltaTime) {
        // 发射子弹
        ShootBullets(deltaTime);
        //更新字体位置
        curHP1.setPosition(position + glm::vec3(-0.18f, -0.3f, 0.0f));
        curHP2.setPosition(position + glm::vec3(-0.1f, -0.3f, 0.0f));
        maxHP.setPosition(position + glm::vec3(0.07f, -0.3f, 0.0f));
        
        //更新血量
        if (health != lasthealth && health >= 0) {
            // 将整数转换为字符串
            
            std::string str = std::to_string(health);
            
            std::string substr1;
            std::string substr2;
            // 提取字符串的两个字符
            if(health >= 10){
                substr1 = str.substr(0, 1);
                substr2 = str.substr(1, 1);
            }
            else {
                substr1 = "0";
                substr2 = str.substr(0, 1);
            }
            const char* str1 = substr1.c_str();    //  取第一个字符
            const char* str2 = substr2.c_str();    //  取第二个字符

            // 确保字符串以空字符结尾
            char buffer1[2] = { str1[0], '\0' };
            char buffer2[2] = { str2[0], '\0' };
 
            curHP1.updateTexture(buffer1);
            curHP2.updateTexture(buffer2);

            lasthealth = health;
        } 
        
    }

    void MoveUp(float distance) {
        if (position.y < -0.5)
        position.y += speed *distance*0.75;     //角色跑到子弹前面会崩溃
    }

    void MoveDown(float distance) {
        if (position.y > -4.3)
        position.y -= speed *distance;
    }

    void MoveLeft(float distance) {
        if (position.x > -1.6)
        position.x -= speed *distance;
    }

    void MoveRight(float distance) {
        if (position.x < 1.6)
        position.x += speed *distance;
    }


private:
    unsigned int textureID;
    unsigned int bulletTextureID;
    Shader& shader; // 使用引用以避免不必要的拷贝
    unsigned int VAO;
    float bulletSpeed;
    float bulletInterval;
    float lastBulletTime;
    std::vector<Bullet> bullets;

    Render curHP1;
    Render curHP2;
    Render maxHP;
    

    
    
    int lasthealth;
    void Setup() {
        float vertices[] = {
            // 位置          // 纹理坐标
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

        // 位置属性
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // 纹理坐标属性
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // 创建子弹对象
        Bullet bullet(glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f), bulletSpeed, bulletTextureID, shader);
        bullets.push_back(bullet);
    }

    void ShootBullets(float deltaTime) {
        float currentTime = glfwGetTime();
        if (currentTime - lastBulletTime > bulletInterval) {
            // 创建并发射子弹
            float bulletSpacing = 0.1f; // 子弹之间的间距
            for (int i = -1; i <= 2; ++i) {
                glm::vec3 bulletPosition = position + glm::vec3(i * bulletSpacing-0.05f, 0.0f, 0.0f);
                if(i==-1 || i==2)
                {
                    bulletPosition += glm::vec3(0.0f, 0.1f, 0.0f);
                }
                Bullet bullet(bulletPosition, glm::vec3(0.0f, 0.6f, 0.0f), bulletSpeed, bulletTextureID, shader);
                bullets.push_back(bullet);
            }

            lastBulletTime = currentTime;
        }
        // 先移除命中的子弹
        for (auto it = bullets.begin(); it != bullets.end(); ) {
            if (it->hasHit) {
                it = bullets.erase(it);  // 删除命中的子弹，并更新迭代器
                it->hasHit = false;
                break;
            }
            else {
                ++it;  // 如果没有命中，继续遍历下一个子弹
            }
        }
        // 检查是否超过数量限制，超过则删除前面的子弹
        if (bullets.size() > 64) {
                bullets.erase(bullets.begin(), bullets.begin() + 32);  
        }
        // 更新子弹
        for (auto& bullet : bullets) {
            bullet.Update(deltaTime);
        }
    }

};
#endif // CHARACTER_H
