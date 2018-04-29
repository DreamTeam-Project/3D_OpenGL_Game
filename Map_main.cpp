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
#include <SOIL.h>

#include "Physics.h"
#include "System.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Skybox.h"

using std::string;
using std::exception;
using glm::vec3;
using std::vector;

static void StartWindow();
static void DrawInWindow(Model_t& Model);
static void LoadLevels();
static void Loading();
static void LoadInfoAboutLevels(Levels& levels);

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow *window);

const char* LoadImage = "textures/please-stand-by.jpg";
vector<string> faces {
	("textures/skybox/nightsky_ft.tga"),
	("textures/skybox/nightsky_bk.tga"),
	("textures/skybox/nightsky_up.tga"),
	("textures/skybox/nightsky_dn.tga"),
	("textures/skybox/nightsky_rt.tga"),
	("textures/skybox/nightsky_lf.tga")
};
const GLuint WIDTH = 1024, HEIGHT = 600;
GLFWwindow* game_window = nullptr;

Camera camera(vec3(0.0f, 0.0f, 3.0f));

GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;

bool firstMouse = true;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

int main() {
	try {
#if _DEBUG
		print("go to main");
#endif
		StartWindow();
#if _DEBUG
		system("pause");
#endif
		return 0;
	}
	catch (Exception_t& exc) {
		print(exc.what());
	}
	catch (exception& exc) {
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
#if _DEBUG 
	print("go to StartWindow");
#endif
	if (!glfwInit()) {
		throw Exception_t(__LINE__, __FILE__, "error initialization GLFW");
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	game_window = glfwCreateWindow(WIDTH, HEIGHT, "Test window", nullptr, nullptr);
	if (game_window == nullptr) {
		glfwTerminate();
		throw Exception_t(__LINE__, __FILE__, "Failed to create GLFW window");
	}

	glfwMakeContextCurrent(game_window);
	glfwSetCursorPosCallback(game_window, MouseCallback);
	glfwSetInputMode(game_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(game_window, ScrollCallback);

	glfwPollEvents();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) {
		throw Exception_t(__LINE__, __FILE__, "Failed to initialize GLEW");
	}

	LoadLevels();
	return;
}

static void DrawInWindow(Model_t& Models) {
#if _DEBUG
	print("\ngo to DrawWindow");
#endif	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
	Shader_t SkyboxShader("Skybox.vs", "Skybox.frag");
	Shader_t Shader("shader.vs", "shader.frag");

	Skybox box;
	box.GenBuffer();
	box.cubemapTexture = box.loadCubemap(faces);

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

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 400.0f);
		glm::mat4 view = camera.GetViewMatrix();
		Shader.setMat4("projection", projection);
		Shader.setMat4("view", view);
		glm::mat4 model;
		model = glm::scale(model, vec3(1.f, 1.f, 1.f));
		Shader.setMat4("model", model);
		Models.Draw(Shader);

		GLfloat fogColor[4] = { 0.5f, 0.5f, 0.5f, 1.0f };

		box.Bind(camera, SkyboxShader, projection);
		glfwSwapBuffers(game_window);
		glfwPollEvents();
	}
}

static void LoadLevels() {
	Levels levels;
	LoadInfoAboutLevels(levels);
	Model_t Models;
	Loading();
	Models.LoadInfoAboutModels(levels.levels_[0].pathLoader_);
	DrawInWindow(Models);
	glfwTerminate();
	return;
}

static void Loading() {
	Shader_t LoaderShader("Load.vs", "Load.frag");
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
	int width, height;
	unsigned char* image = SOIL_load_image(LoadImage, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	SOIL_free_image_data(image);
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

static void LoadInfoAboutLevels(Levels& levels) {
	std::ifstream fin(LoadFile);
	if (!fin.is_open()) {
		throw Exception_t(__LINE__, __FILE__, "error open file Load.file");
	}
	string name_buf = "";
	string path_buf = "";
	print(string("\ntry to read: ") + LoadFile);
	while (!fin.eof()) {
		getStringFromFile(fin, name_buf);
		if (name_buf == "end_of_file" || name_buf == "null") {
			throw Exception_t(__LINE__, __FILE__, "error format Load.file");
		}
		getStringFromFile(fin, path_buf);
		if (path_buf == "end_of_file" || name_buf == "null") {
			break;
		}
		levels.push_back(Level(name_buf, path_buf));
	}
	if (levels.levels_.size() == 0) {
		throw Exception_t(__LINE__, __FILE__, "LoadFile.file is empty");
	}
	print("success\n");
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
