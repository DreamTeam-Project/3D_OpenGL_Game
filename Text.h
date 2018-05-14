#ifndef TEXT_H
#define TEXT_H

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <vector>
#include <map>
#include <ft2build.h>
#include "Shader.h"
#include "System.h"

#include FT_FREETYPE_H

using glm::ivec2;
using glm::vec3;
using std::map;
using std::string;
using std::vector;

struct SysStrings {
	SysStrings(const string& text, float x, float y, float scale, const vec3& color = vec3(1.0f)) :
		text_(text), x_(x), y_(y), scale_(scale), color_(color) {  }
	string text_;
	float x_;
	float y_;
	float scale_;
	vec3 color_;
};

struct Char {
	uint TextureID;
	ivec2 Size;
	ivec2 Bearing;
	uint Advance;
};

class GameText {
public:
	GameText(const string& path, uint height = HEIGHT, uint width = WIDTH);
	~GameText();
	void RenderText(const string& text, float x, float y, float scale, const vec3& color = vec3(1.0f));
	void RenderText(const vector<SysStrings>& text);

private:
	void LoadFonts(const string& path, uint height = HeightFont, uint width = WidthFont);
	GameText();

	GameShader Shader;
	map<char, Char> Characters;
	uint VAO, VBO;
};

#endif