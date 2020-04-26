#pragma once
#include <tuple>
#include <GL/glew.h>
#include <ft2build.h>
#include <iostream>
#include <map>
#include "TextureAtlas.h"
#include FT_FREETYPE_H

struct CharacterInfo {
	std::tuple<int, int> _size;       // Size of glyph
	std::tuple<int, int> _bearing;    // Offset from baseline to left/top of glyph
	GLuint _advance;    // Offset to advance to next glyph

	CharacterInfo& operator=(const CharacterInfo& rhs) {
		_size = rhs._size;
		_bearing = rhs._bearing;
		_advance = rhs._advance;
		return *this;
	}
};

class GlyphsLoader {
public:
	static GlyphsLoader& instance();
	void initialize(std::string fontPath, std::tuple<int, int> symbolsCodes);
	void bufferSymbols(int symbolHeight);
	std::map<FT_ULong, CharacterInfo> getCharacters();
	std::map<FT_ULong, std::tuple<float, float> > getCharactersAtlasPosition();
	std::shared_ptr<TextureAtlas> getAtlas();
	float getLineSpace();
private:
	float _lineSpace;
	int _symbolHeight;
	std::string _fontPath;
	std::tuple<int, int> _symbolsCodes;
	std::shared_ptr<TextureAtlas> _charactersAtlas;
	std::map<FT_ULong, std::tuple<float, float> > _charactersAtlasPosition;
	std::map<FT_ULong, CharacterInfo> _characters;
};


