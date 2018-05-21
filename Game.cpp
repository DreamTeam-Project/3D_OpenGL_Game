#define GLEW_STATIC

#include <cstdio>
#include <gl\glew.h>
#include <gl\GL.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <iostream>
#include <exception>
#include <vector>
#include <stb_image_aug.h>
#include <string>

#include "System.h"
#include "Shader.h"
#include "Camera.h"
#include "Manager.h"
#include "Image.h"

using std::string;
using std::exception;
using glm::vec3;
using std::vector;
static void StartWindow();
static void DrawInWindow();

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void ProcessInputInGame(GLFWwindow *window);

GLFWwindow* game_window = nullptr;

GameManager* game_man;

Camera camera(vec3(10.0f, 10.0f, 10.0f));

GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
	try {
#if DEBUG_GAME
		print("go to main");
#endif
		StartWindow();
#if DEBUG_GAME
		system("pause");
#endif
		return 0;
	}
	catch (const exception& exc) {
		print(exc.what());
	}
	catch (...) {
		print("Error default\n");
	}
#if _DEBUG
	system("pause");
#endif
}

static void StartWindow() {
#if DEBUG_GAME 
	print("go to StartWindow");
#endif
	if (!glfwInit()) {
		throw GameException(__LINE__, __func__, "Error: initialization GLFW");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	game_window = glfwCreateWindow(WIDTH, HEIGHT, "Test window", nullptr, nullptr);
	if (game_window == nullptr) {
		glfwTerminate();
		throw GameException(__LINE__, __func__, "Error: Failed to create GLFW window");
	}

	glfwMakeContextCurrent(game_window);
	glfwSetCursorPosCallback(game_window, MouseCallback);
	glfwSetInputMode(game_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(game_window, ScrollCallback);

	glfwPollEvents();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw GameException(__LINE__, __func__, "Error: Failed to initialize GLEW");
	}

	DrawInWindow();


	glfwTerminate();
}

static void DrawInWindow() {
#if DEBUG_GAME
	print("\ngo to DrawWindow");
#endif	
	Image Loading(LoadImage);
	Loading.RenderImage(true);

	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GameManager Manager;
	game_man = &Manager;
	while (!glfwWindowShouldClose(game_window) && Manager.GameMenu(game_window, Loading)) {

		while (!glfwWindowShouldClose(game_window) && Manager.play) {
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			ProcessInputInGame(game_window);
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 400.0f);
			mat4 view = camera.GetViewMatrix();

			Manager.RenderWorld(projection, view, camera, deltaTime);
			
			glfwSwapBuffers(game_window);
			glfwPollEvents();
		}

		Loading.RenderImage(true);
		Manager.EndLevel();
	}
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void ProcessInputInGame(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		camera.ProcessKeyboard(FORWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		camera.ProcessKeyboard(LEFT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		camera.ProcessKeyboard(RIGHT, deltaTime);
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		camera.position->jump();
	}
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		printf("just R\n");
		static int press = 0;
		press++;
		if (press % 100 == 11) {
			phys_body* tmp = camera.position->aim(game_man->real_world_);
			if (tmp == nullptr) {
				
				printf("null\n");
				return;
			}
				
			game_man->AllModels.push_back(new GameModel(game_man->AllModels[0], tmp,
				vec3(0, 0, 0), vec3(1, 1, 1), true));
			printf("created bullet\n");
		}
		
	}
}