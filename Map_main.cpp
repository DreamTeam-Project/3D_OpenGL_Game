#define GLEW_STATIC

#include <cstdio>
#include <gl\glew.h>
#include <gl\GL.h>
#include <GLFW\glfw3.h>
#include <glm\glm.hpp>
#include <iostream>
#include <exception>
#include <vector>
#include <string>

#include "Physics.h"
#include "System.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

using std::string;
using std::exception;
using glm::vec3;

static void StartWindow();
static void DrawInWindow();

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

GLFWwindow* game_window = nullptr;
const GLuint WIDTH = 800, HEIGHT = 600;

Camera camera(vec3(0.0f, 0.0f, 3.0f));
vec3 cameraPos = vec3(0.0f, 0.0f, 3.0f);
vec3 cameraFront = vec3(0.0f, 0.0f, -1.0f);
vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);

GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

bool firstMouse = true;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

struct Map static_models = {
	//{ "obj/Enginer/MP_US_Engi.3DS" },
	//{ "obj/l00_intro/l00_intro.3ds" },
	{ "obj/island/Small Tropical Island.obj", "obj/test/p3.obj"},
	//{ "obj/island/Small Tropical Island.obj" },
	{ vec3(0.0f, -100.0f, 0.0f),  vec3(0.0f, 50.0f, 0.0f) }
};

int main() {
#if _DEBUG
	printf("go to main\n");
#endif
	StartWindow();
#if _DEBUG
	system("pause");
#endif
	return 0;
}

static void StartWindow() {
	try {
#if _DEBUG 
		printf("go to StartWindow\n");
#endif
		if (!glfwInit()) {
			throw Exception_t(__LINE__, __FILE__, "error initialization GLFW");
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		game_window = glfwCreateWindow(WIDTH, HEIGHT, "Test window", nullptr, nullptr);
		if (game_window == nullptr) {
			glfwTerminate();
			throw Exception_t(__LINE__, __FILE__, "Failed to create GLFW window");
		}

		glfwMakeContextCurrent(game_window);
		glfwSetCursorPosCallback(game_window, MouseCallback);
		glfwSetInputMode(game_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetScrollCallback(game_window, ScrollCallback);
		glfwSetFramebufferSizeCallback(game_window, FramebufferSizeCallback);

		glfwPollEvents();
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);

		glewExperimental = GL_TRUE;
		if (glewInit() != GLEW_OK) {
			throw Exception_t(__LINE__, __FILE__, "Failed to initialize GLEW");
		}

		DrawInWindow();
		return;
	}
	catch (Exception_t& exc) {
		//PrintException(exc.what());
		printf("%s\n", exc.what().c_str());
	}
	catch (exception& exc) {
		//PrintException(string(exc.what()));
		printf("%s\n", exc.what());
	}
}

static void DrawInWindow() {
	try {
#if _DEBUG
		printf("go to DrawWindow\n");
#endif	
		Shader_t Shader("shader.vs", "shader.frag");
		Model_t Model(&static_models);
		//Physics tmp(Model.meshes_c[0].vertices_c.size(), Model.meshes_c[0].vertices_c);
		while (!glfwWindowShouldClose(game_window)) {
			GLfloat currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			processInput(game_window);

			glClearColor(0.01f, 0.05f, 0.01f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			Shader.Use();

			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			Shader.setMat4("projection", projection);
			Shader.setMat4("view", view);

			glm::mat4 model;
			model = glm::translate(model, vec3(0.0f, -1.75f, 0.0f));
			model = glm::scale(model, vec3(0.2f, 0.2f, 0.2f));
			Shader.setMat4("model", model);
			Model.Draw(Shader);
			glfwSwapBuffers(game_window);
			glfwPollEvents();
		}
		glfwTerminate();
	}
	catch (Exception_t& exc) {
		//PrintException(exc.what());
		printf("%s\n", exc.what().c_str());
	}
	catch (exception& exc) {
		//PrintException(string(exc.what()));
		printf("%s\n", exc.what());
	}
	return;
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

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
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
}