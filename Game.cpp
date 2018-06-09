//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
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

GLFWwindow* game_window = nullptr;
Camera camera(vec3(0.0f, 10.0f, 0.0f));

GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
	try {
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
#if TERMINAL
	system("pause");
#endif
}

static void StartWindow() {
	if (!glfwInit()) {
		throw GameException(__LINE__, __func__, "Error: initialization GLFW");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	game_window = glfwCreateWindow(WIDTH, HEIGHT, "Game", nullptr, nullptr);
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
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GameManager Manager;
	while (!glfwWindowShouldClose(game_window) && Manager.GameMenu(game_window)) {

		while (!glfwWindowShouldClose(game_window) && Manager.Status()) {
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 400.0f);
			mat4 view = camera.GetViewMatrix();

			Manager.RenderWorld(projection, view, camera, deltaTime);
			
			glfwSwapBuffers(game_window);
			glfwPollEvents();
		}
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