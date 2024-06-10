#include <glad/glad.h>
 #include <iostream>
#include <GLFW/glfw3.h>
#include <SDL/SDL.h>
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
# include "shader.h"
#include "Camera.h"
#define STB_IMAGE_IMPLEMENTATION
#include "src/stb_image.h"
#include "character.h"
#include "bullet.h"
#include "enemy.h"
#include "render.h"
unsigned int loadTexture(char const* path);
// SCR_WIDTH = ...
Camera camera(glm::vec3(0.0f, 0.0f, 6.0f));

const unsigned int SCR_WIDTH = 600;
const unsigned int SCR_HEIGHT = 1000;

float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;
float deltaTime = 0.0f; // 两帧的时间差
float lastFrame = 0.0f; // 上一帧的时间
float playerRange = 0.15f;	//玩家伤害判断范围
float enemyRange = 0.25f;


// 修改ProcessInput函数，加入移动摄像机的代码
void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
bool CheckCollision(const Character& player, Bullet& bullet) {
	if (bullet.hasHit) return false; // 如果子弹已经命中，则不进行碰撞检测

	bool collisionX = player.position.x + playerRange >= bullet.position.x &&
		player.position.x - playerRange <= bullet.position.x;
	bool collisionY = player.position.y + playerRange >= bullet.position.y &&
		player.position.y - playerRange <= bullet.position.y;
	return collisionX && collisionY;
}
bool EnemyCollision(const Enemy& player, Bullet& bullet) {
	if (bullet.hasHit) return false; // 如果子弹已经命中，则不进行碰撞检测

	bool collisionX = player.position.x + enemyRange >= bullet.position.x &&
		player.position.x - enemyRange <= bullet.position.x;
	bool collisionY = player.position.y + enemyRange + 0.4f >= bullet.position.y &&
		player.position.y - enemyRange + 0.4f <= bullet.position.y;
	return collisionX && collisionY;
}

// 音频回调函数
void MyAudioCallback(void* userdata, Uint8* stream, int len);
bool InitAudio(const char* filename, SDL_AudioSpec* audioSpec);
void PlayAudio();
void CloseAudio(SDL_AudioSpec* audioSpec);


int main(int argc, char* argv[])
 {
	float characterspeedf;
	printf("请调整速度（推荐10）：");
	scanf_s("%f", &characterspeedf);
	printf("\n当前速度为%f\n\n", characterspeedf);
	glfwInit();
		// 设置OpenGL版本为3.3
		 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		// 设置OpenGL为核心模式 (只能使用OpenGL功能的一个子集)
		 glfwWindowHint(GLFW_OPENGL_PROFILE,
			GLFW_OPENGL_CORE_PROFILE);
		 // 或者可以使用向后兼容的OpenGL功能
		 //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		 // 创建窗口对象
			  // 参数：宽度，高度，窗口标题，不使用全屏，不共享资源
			  GLFWwindow * window = glfwCreateWindow(SCR_WIDTH,
				 SCR_HEIGHT, "StarRail Project", NULL, NULL);
			  if (window == NULL)
			  {
			  std::cout << "Failed to create GLFW window" << std
				 ::endl;
				 glfwTerminate();
				  return -1;
				 }
		  glfwMakeContextCurrent(window);
		  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 关闭鼠标显示
			  // GLAD初始化
			  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress
			 ))
			 {
			  std::cout << "Failed to initialize GLAD" << std::
				 endl;
				 return -1;
				  }
		  // 视口：渲染窗口大小
			  glViewport(0, 0, 600, 1000);
			  glEnable(GL_DEPTH_TEST);
			  HWND hwnd = glfwGetWin32Window(window);
			  HKL hkl = LoadKeyboardLayout(L"00000409", KLF_ACTIVATE);
			  if (hkl != NULL) {
				  ActivateKeyboardLayout(hkl, KLF_SETFORPROCESS);
			  }

			  SDL_AudioSpec audioSpec;
			  Uint8* audioBuffer;
			  Uint32 audioLength;
			  if (!InitAudio("res/music/bgm.wav", &audioSpec)) {
				  return -1;
			  }
			  PlayAudio();
			  Shader shader_diamond("res/expr2.vs", "res/expr2.fs");
			  unsigned int texture = loadTexture("res/diamond.png");
			  unsigned int bulletTexture = loadTexture("res/bullet.png");
			  unsigned int enemyTexture = loadTexture("res/enemy.png");
			  unsigned int enemyTexture1 = loadTexture("res/enemy1.png");
			  unsigned int enemyTexture2 = loadTexture("res/enemy2.png");
			  unsigned int BoomTexture = loadTexture("res/boom.png");
			  unsigned int enemyBulletTexture = loadTexture("res/enemybullet.png");
			  shader_diamond.use();
			  shader_diamond.setInt("texture1", 0);
		//	  Render render(glm::vec3(0.0f, -3.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), "character", shader_diamond);
		 //让角色底一点防止边框太明显
			  Character character(glm::vec3(0.0f, -3.0f, -0.01f), glm::vec3(0.8f, 0.8f, 0.8f), texture, bulletTexture,shader_diamond);
			  Enemy enemy(glm::vec3(0.0f, 0.25f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), enemyTexture, enemyBulletTexture, shader_diamond);
			  character.speed = characterspeedf / 10 * character.speed;
			  // main 函数结尾添加循环
			  while (!glfwWindowShouldClose(window)) {
				  {
					  float currentFrame = static_cast<float>(glfwGetTime());
					  deltaTime = currentFrame - lastFrame;
					  lastFrame = currentFrame;
					  processInput(window);
				  }
				  // 清理颜色缓冲和深度缓冲
				  glClearColor(0.0f, 0.8f, 0.8f, 1.0f);
				  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				  shader_diamond.use();
				  shader_diamond.setMat4("model", glm::mat4(1.0f));
				  shader_diamond.setMat4("view", camera.GetViewMatrix());
				  shader_diamond.setMat4("projection", glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f));
				  

				  // 移动角色
				  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
					  character.MoveUp(0.01f); // 向上移动
				  }
				  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
					  character.MoveDown(0.01f); // 向下移动
				  }
				  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
					  character.MoveLeft(0.01f); // 向左移动
				  }
				  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
					  character.MoveRight(0.01f); // 向右移动
				  }
				  if (enemy.health == 70 || enemy.health == 40) {
					  enemy.textureID = BoomTexture;
				  }
				  if (enemy.health == 69) {
					  enemy.size = glm::vec3(1.2f, 1.2f, 1.2f);
					  enemy.textureID = enemyTexture2;
				  }
				  if (enemy.health == 39) {
					  enemy.textureID = enemyTexture1;
					  enemy.bulletInterval = 0.04f;
					  enemy.bulletSpeed = 1.6f;
				  }

				  // 绘制角色
				  // 启用混合功能，先画底下的图层
				  glEnable(GL_BLEND);
				  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				  //更新位置
					character.Update(deltaTime);
					enemy.Update(deltaTime);
					if (character.health < 1) {
						if (enemy.health > 50) {
							printf("负分！真没用(●'ε'●)\n");
						}
						else
						{
							printf("零分！杂鱼(●'ω'●)\n");
						}
						break;
					}
					if (enemy.health < 1) {
						if (character.health > 20) {
							printf("把钱还我，我不玩了（跨起个爱莉批脸）(●'ε'●)\n");
						}
						else
						{
							printf("别以为这样就赢了(●'ω'●)\n");
						}
						break;
					}
					// 获取敌人的子弹
					auto& enemyBullets = enemy.GetBullets();
					// 获取玩家的子弹
					auto& characterBullets = character.GetBullets();
					// 检查每个子弹是否与玩家碰撞
					for (auto& bullet : enemyBullets) {
						if (CheckCollision(character, bullet)) {
							bullet.hasHit = true; // 设置标志位
							character.TakeDamage(1); // 示例伤害值
						}
					}
					// 检查每个子弹是否与敌人碰撞
					for (auto& bullet : characterBullets) {
						//printf("%d  ", bullet.hasHit);
						if (EnemyCollision(enemy, bullet)) {
							//printf("%f	%f碰撞%d\n", bullet.position.x, bullet.position.y, bullet.hasHit);
							bullet.hasHit = true; // 设置标志位
							enemy.TakeDamage(1); // 示例伤害值
						}
					}
					
					//绘制
				  character.Draw();
				  enemy.Draw();
				  
				  //bullet.Update(deltaTime);
				  //bullet.Draw();
				  // 检查并调用事件，交换缓冲
				  glfwSwapBuffers(window);
				  glfwPollEvents();
			  }
			  CloseAudio(&audioSpec);

			  glfwDestroyWindow(window);
			  Sleep(10000);
			 return 0;
			
}

unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID); // 先绑定纹理对象

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

	return textureID;
}

// 音频回调函数
void MyAudioCallback(void* userdata, Uint8* stream, int len) {
	static Uint32 audioPos = 0;
	Uint32 audioLen = *(Uint32*)userdata; // 获取音频长度
	Uint8* audioBuffer = ((Uint8*)userdata) + sizeof(Uint32); // 获取音频数据缓冲区

	if (audioPos + len > audioLen) {
		len = audioLen - audioPos;
	}

	SDL_memcpy(stream, audioBuffer + audioPos, len);
	audioPos += len;

	if (audioPos >= audioLen) {
		audioPos = 0; // 循环播放
	}
}

bool InitAudio(const char* filename, SDL_AudioSpec* audioSpec) {
	// 初始化 SDL 音频子系统
	if (SDL_Init(SDL_INIT_AUDIO) < 0) {
		std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	// 加载 WAV 音频文件
	Uint8* audioBuffer;
	Uint32 audioLength;
	if (SDL_LoadWAV(filename, audioSpec, &audioBuffer, &audioLength) == NULL) {
		std::cerr << "Failed to load WAV file! SDL_Error: " << SDL_GetError() << std::endl;
		return false;
	}

	// 分配新的缓冲区以保存长度信息和音频数据
	Uint8* completeBuffer = (Uint8*)SDL_malloc(audioLength + sizeof(Uint32));
	SDL_memcpy(completeBuffer, &audioLength, sizeof(Uint32));
	SDL_memcpy(completeBuffer + sizeof(Uint32), audioBuffer, audioLength);

	SDL_FreeWAV(audioBuffer); // 释放原始缓冲区

	audioSpec->callback = MyAudioCallback;
	audioSpec->userdata = completeBuffer; // 将完整的缓冲区作为 userdata 传递

	// 打开音频设备
	if (SDL_OpenAudio(audioSpec, NULL) < 0) {
		std::cerr << "Failed to open audio! SDL_Error: " << SDL_GetError() << std::endl;
		SDL_free(completeBuffer);
		return false;
	}
	return true;
}

void PlayAudio() {
	// 开始播放音频
	SDL_PauseAudio(0);
}

void CloseAudio(SDL_AudioSpec* audioSpec) {
	SDL_CloseAudio();
	SDL_free((Uint8*)audioSpec->userdata);
	SDL_Quit();
}





