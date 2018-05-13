#ifndef IMAGE_H
#define IMAGE_H

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stb_image_aug.h>
#include <string>

#include "System.h"
#include "Shader.h"

using std::string;

const GLuint indices[] = {
	0, 1, 3,
	1, 2, 3
};
const GLfloat vertices[] = {
	1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,
	1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f
};

class Image {
public:
	Image();
	Image(const string& path);
	~Image();
	void RenderImage(bool swap = false) const;
	void LoadImage(const string& path);

private:
	GameShader Shader;
	uint VAO, VBO, EBO;
	uint texture;
};

#endif
