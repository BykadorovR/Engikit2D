#pragma once
#include "platformGL.h"
#include "Common.h"
#include "Buffer.h"
#include "Matrix.h"
#include <ft2build.h>
#include <assert.h>
#include "Shader.h"
#include FT_FREETYPE_H

struct Character {
	GLuint textureID;  // ID handle of the glyph texture
	std::pair<int, int> size;       // Size of glyph
	std::pair<int, int> bearing;    // Offset from baseline to left/top of glyph
	GLuint advance;    // Offset to advance to next glyph
};

class TextLoader {
public:
	void bufferSymbols() {
		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		FT_Face face;
		if (FT_New_Face(ft, "../data/fonts/arial.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, 48);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

		for (GLubyte c = 0; c < 128; c++)
		{
			// Load character glyph 
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
				continue;
			}
			// Generate texture
			GLuint texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
			);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use
			Character character = {
				texture,
				std::pair<int, int>(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				std::pair<int, int>(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			_characters.insert(std::pair<GLchar, Character>(c, character));
		}
	}

	void initialize() {
		Shader shader;
		_program = shader.buildProgramFromAsset("../data/shaders/text.vsh", "../data/shaders/text.fsh");
		glGenVertexArrays(1, &_VAO);
		glGenBuffers(1, &_VBO);
		glBindVertexArray(_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, _VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void render(std::string text, GLfloat x, GLfloat y, GLfloat scale, std::vector<float> color) {
		glUseProgram(_program);
		glUniform3f(glGetUniformLocation(_program, "textColor"), color[0], color[1], color[2]);
		glActiveTexture(GL_TEXTURE0);
		glBindVertexArray(_VAO);

		// Iterate through all characters
		std::string::const_iterator c;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = _characters[*c];

			GLfloat xpos = x + ch.bearing.first * scale;
			xpos /= (float) resolution.first;
			GLfloat ypos = y - (ch.size.second - ch.bearing.second) * scale;
			ypos /= (float) resolution.second;

			GLfloat w = ch.size.first * scale;
			w /= (float) resolution.first;
			GLfloat h = ch.size.second * scale;
			h /= (float) resolution.second;
			// Update VBO for each character
			GLfloat vertices[6][4] = {
				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos,     ypos,       0.0, 1.0 },
				{ xpos + w, ypos,       1.0, 1.0 },

				{ xpos,     ypos + h,   0.0, 0.0 },
				{ xpos + w, ypos,       1.0, 1.0 },
				{ xpos + w, ypos + h,   1.0, 0.0 }
			};
			// Render glyph texture over quad
			glBindTexture(GL_TEXTURE_2D, ch.textureID);
			// Update content of VBO memory
			glBindBuffer(GL_ARRAY_BUFFER, _VBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad
			glDrawArrays(GL_TRIANGLES, 0, 6);
			// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
			x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
		}
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	GLuint _VAO, _VBO;
	GLuint _program;
	std::map<GLchar, Character> _characters;
};