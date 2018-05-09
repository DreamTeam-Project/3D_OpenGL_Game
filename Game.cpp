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
#include "Skybox.h"
#include "Manager.h"

using std::string;
using std::exception;
using glm::vec3;
using std::vector;

static void StartWindow();
static void DrawInWindow();
static void Loading();

void SetSpotLights(GameShader& shader);
void SetGlobalLight(GameShader& shader);
void SetPointLights(GameShader& shader);

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

const GLuint WIDTH = 1024, HEIGHT = 600;
GLFWwindow* game_window = nullptr;

Camera camera(vec3(0.0f, 40.0f, 3.0f));
GameManager Manager;

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

	bool play = true;
	while (play) {
		Manager.LoadInfoAboutLevels();
		size_t levelNumber = Manager.ChooseLevel();
		Loading();
		Manager.LoadInfoAboutModels(levelNumber);
		DrawInWindow();
		play = Manager.BeOrNotToBe();
	}
	glfwTerminate();
}

static void DrawInWindow() {
#if DEBUG_GAME
	print("\ngo to DrawWindow");
#endif	
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	GameShader SkyboxShader("Skybox.vs", "Skybox.fs");
	GameShader Shader("Light.vs", "Light.fs");

	Skybox box;
	box.GenBuffer();
	box.cubemapTexture = box.loadCubemap(DarkStormy);

	SkyboxShader.Use();
	SkyboxShader.setInt("skybox", 0);

	while (!glfwWindowShouldClose(game_window)) {
		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(game_window);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Shader.Use();

		mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 400.0f);
		mat4 view = camera.GetViewMatrix();

		Shader.setMat4("projection", projection);
		Shader.setMat4("view", view);
		Shader.setVec3("viewPos", camera.Position);

		SetGlobalLight(Shader);
		SetPointLights(Shader);
		SetSpotLights(Shader);

		for (auto& it : Manager.AllModels) {
			it->SetShaderParameters(Shader);
			it->Draw(Shader);
		}

		box.Bind(camera, SkyboxShader, projection);
		glfwSwapBuffers(game_window);
		glfwPollEvents();
	}
}

void SetGlobalLight(GameShader& shader) {
	shader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	shader.setVec3("dirLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("dirLight.diffuse", 0.2f, 0.2f, 0.2f);
	shader.setVec3("dirLight.specular", 0.2f, 0.2f, 0.2f);
}

void SetPointLights(GameShader& shader) {
	shader.setVec3("pointLights[0].position", vec3(10, 10, 10));
	shader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	shader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	shader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("pointLights[0].constant", 1.0f);
	shader.setFloat("pointLights[0].linear", 0.09);
	shader.setFloat("pointLights[0].quadratic", 0.032);
}

void SetSpotLights(GameShader& shader) {
	shader.setVec3("spotLight.position", camera.Position);
	shader.setVec3("spotLight.direction", camera.Front);
	shader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	shader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	shader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	shader.setFloat("spotLight.constant", 1.0f);
	shader.setFloat("spotLight.linear", 0.09);
	shader.setFloat("spotLight.quadratic", 0.032);
	shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(20.0f)));
	shader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(25.0f)));
}

static void Loading() {
	GameShader LoaderShader("Load.vs", "Load.fs");
	GLfloat vertices[] = {
		1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
	};
	GLuint indices[] = { 
		0, 1, 3,
		1, 2, 3 
	};
	GLuint VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0); 
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	int width, height, nrChannels;
	unsigned char *image = stbi_load(LoadImage.c_str(), &width, &height, &nrChannels, 0);
	if (image) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		stbi_image_free(image);
	}
	else {
		stbi_image_free(image);
		throw GameException(__LINE__, __func__, string("Error load: path: ") + LoadImage.c_str(),
			string("Why: ") + string(stbi_failure_reason()));
	}
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glfwPollEvents();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, texture);
	LoaderShader.Use();
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glfwSwapBuffers(game_window);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
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
