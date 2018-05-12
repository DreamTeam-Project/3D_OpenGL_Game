#ifndef TEXT_H
#define TEXT_H

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <map>
#include <ft2build.h>
#include "Shader.h"
#include "System.h"

#include FT_FREETYPE_H

using glm::ivec2;
using glm::vec3;
using std::map;
using std::string;

struct Character {
	uint TextureID;
	ivec2 Size;
	ivec2 Bearing;
	uint Advance;
};

class GameText {
public:
	GameText(const GameShader& shader, uint height = HEIGHT, uint width = WIDTH);
	void LoadFonts(const string& path = FontFile, uint height = HeightFont, uint width = WidthFont);
	void RenderText(const GameShader& shader, const string& text, float x, float y, float scale, const vec3& color = vec3(1.0f));
private:
	//GameShader Shader;
	map<char, Character> Characters;
	uint VAO, VBO;
};

#endif