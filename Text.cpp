#include "Text.h"

GameText::GameText(uint height, uint width) : Shader("Text.vs", "Text.fs") {
	Shader.Use();
	Shader.setMat4("projection", glm::ortho(0.0f, (float)HEIGHT, 0.0f, (float)WIDTH));
	Shader.setInt("text", 0);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void GameText::LoadFonts(const string& path, uint height, uint width) {
	Characters.clear();
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		throw GameException(__LINE__, __func__, "Error, FreeType: Could not init FreeType Library");
	}
	FT_Face face;
	if (FT_New_Face(ft, path.c_str(), 0, &face)) {
		throw GameException(__LINE__, __func__, "Error, FreeType: Failed to load font");
	}
	FT_Set_Pixel_Sizes(face, width, height);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	for (GLubyte c = 0; c < 128; c++) {
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
#ifdef DEBUG_FONTS
			print("Error, FreeType: Failed to load Glyph");
#endif
			continue;
		}
		uint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Char character = {
			texture,
			ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<char, Char>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

void GameText::RenderText(const string& text, float x, float y, float scale, const vec3& color) {
	Shader.Use();
	Shader.setVec3("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);
	for (auto& c : text) {
		Char ch = Characters[c];
		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		float vertices[6][4] = {
		{ xpos		, ypos + h	, 0.0, 0.0 },
		{ xpos		, ypos		, 0.0, 1.0 },
		{ xpos + w	, ypos		, 1.0, 1.0 },

		{ xpos		, ypos + h	, 0.0, 0.0 },
		{ xpos + w	, ypos		, 1.0, 1.0 },
		{ xpos + w	, ypos + h	, 1.0, 0.0 }
		};
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void GameText::RenderText(const vector<SysStrings>& text) {
	for (auto& it : text) {
		RenderText(it.text_, it.x_, it.y_, it.scale_, it.color_);
	}
}

GameText::GameText(const string& path, uint height, uint width) : GameText(height, width) {
	LoadFonts(path);
}

GameText::~GameText() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}