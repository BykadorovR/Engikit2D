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
	void bufferSymbols(int symbolHeight) {
		_symbolHeight = symbolHeight;

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
			std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

		FT_Face face;
		if (FT_New_Face(ft, "../data/fonts/arial.ttf", 0, &face))
			std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
		FT_Set_Pixel_Sizes(face, 0, symbolHeight);
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

	int _symbolHeight;
	std::map<GLchar, Character> _characters;
};