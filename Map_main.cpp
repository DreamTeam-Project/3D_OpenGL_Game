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
#include "Types.h"
#include "Shader.h"
#include "Camera.h"

using std::string;
using std::exception;

void StartWindow();
void DrawInWindow();
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode);
void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void MouseCallback(GLFWwindow* window, double xpos, double ypos);
void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

GLFWwindow* game_window = nullptr;
const GLuint WIDTH = 800, HEIGHT = 600;
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
GLfloat yaw = -90.0f;	// Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right (due to how Eular angles work) so we initially rotate a bit to the left.
GLfloat pitch = 0.0f;
GLfloat lastX = WIDTH / 2.0;
GLfloat lastY = HEIGHT / 2.0;
bool keys[1024];
bool firstMouse = true;
GLfloat deltaTime = 0.0f;	// Time between current frame and last frame
GLfloat lastFrame = 0.0f;  	// Time of last frame

int main() {
	printf("main\n");
	StartWindow();
	system("pause");
	return 0;
}

void StartWindow() {
	try {
		printf("StartWindow\n");
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
		glfwSetKeyCallback(game_window, KeyCallback);
		glfwSetCursorPosCallback(game_window, MouseCallback);
		glfwSetInputMode(game_window, GLFW_STICKY_KEYS, GL_TRUE);
		glfwSetInputMode(game_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(game_window, WIDTH / 2, HEIGHT / 2);
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
		exc.print();
	}
	catch (exception& e) {
		printf("Standard exception: %s\n", e.what());
	}
}

void DrawInWindow() {
	try {
		printf("DrawWindow\n");


		GLuint VAO, VBO;
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		// Create and compile our GLSL program from the shaders
		//GLuint programID = LoadShaders("TransformVertexShader.vertexshader", "TextureFragmentShader.fragmentshader");
		// Get a handle for our "MVP" uniform
		//GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		// Load the texture
		//GLuint Texture = loadDDS("uvmap.DDS");
		// Get a handle for our "myTextureSampler" uniform
		//GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");
		Shader_t ourShader("shader.vs", "shader.frag");
		// Read our .obj file
		//std::vector<glm::vec3> vertices;
		//std::vector<glm::vec2> uvs;
		//std::vector<glm::vec3> normals; // Won't be used at the moment.
		//bool res = loadOBJ("cube.obj", vertices, uvs, normals);
		// Load it into a VBO
		GLuint vertexbuffer;
		//glGenBuffers(1, &vertexbuffer);
		//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		//glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

		//GLuint uvbuffer;
		//glGenBuffers(1, &uvbuffer);
		//glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		//glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
		
		while (!glfwWindowShouldClose(game_window)) {

			//// Clear the screen
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			//// Use our shader
			//glUseProgram(programID);

			//// Compute the MVP matrix from keyboard and mouse input
			//computeMatricesFromInputs();
			//glm::mat4 ProjectionMatrix = getProjectionMatrix();
			//glm::mat4 ViewMatrix = getViewMatrix();
			//glm::mat4 ModelMatrix = glm::mat4(1.0);
			//glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

			//// Send our transformation to the currently bound shader, 
			//// in the "MVP" uniform
			//glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

			//// Bind our texture in Texture Unit 0
			//glActiveTexture(GL_TEXTURE0);
			//glBindTexture(GL_TEXTURE_2D, Texture);
			//// Set our "myTextureSampler" sampler to use Texture Unit 0
			//glUniform1i(TextureID, 0);

			//// 1rst attribute buffer : vertices
			//glEnableVertexAttribArray(0);
			//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
			//glVertexAttribPointer(
			//	0,                  // attribute
			//	3,                  // size
			//	GL_FLOAT,           // type
			//	GL_FALSE,           // normalized?
			//	0,                  // stride
			//	(void*)0            // array buffer offset
			//);

			//// 2nd attribute buffer : UVs
			//glEnableVertexAttribArray(1);
			//glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
			//glVertexAttribPointer(
			//	1,                                // attribute
			//	2,                                // size
			//	GL_FLOAT,                         // type
			//	GL_FALSE,                         // normalized?
			//	0,                                // stride
			//	(void*)0                          // array buffer offset
		}
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glfwTerminate();
	}
	catch (Exception_t& exc) {
		exc.print();
	}
	catch (exception& e) {
		printf("Standard exception: %s\n", e.what());
	}
	return;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	return;
}

void DoMovement() {
	GLfloat cameraSpeed = 5.0f * deltaTime; 
	if (keys[GLFW_KEY_W]) {
		cameraPos += cameraSpeed * cameraFront;
	}
	if (keys[GLFW_KEY_S]) {
		cameraPos -= cameraSpeed * cameraFront;
	}
	if (keys[GLFW_KEY_A]) {
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
	if (keys[GLFW_KEY_D]) {
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}
}

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse) {
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;
	GLfloat sensitivity = 0.05;
	xoffset *= sensitivity;
	yoffset *= sensitivity;
	yaw += xoffset;
	pitch += yoffset;
	// Make sure that when pitch is out of bounds, screen doesn't get flipped
	if (pitch > 89.0f) {
		pitch = 89.0f;
	}
	if (pitch < -89.0f) {
		pitch = -89.0f;
	}
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}