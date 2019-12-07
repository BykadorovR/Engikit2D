#pragma once
#include <tuple>
#include <GL/glew.h>
#include <ft2build.h>
#include <iostream>
#include <map>
#include "TextureAtlas.h"
#include FT_FREETYPE_H

class GlyphsLoader {
public:
	GlyphsLoader();
	void bufferSymbols(int symbolHeight);
	std::map<GLchar, FT_Face> getCharacters();
	std::map<GLchar, std::tuple<float, float> > getCharactersAtlasPosition();
	std::shared_ptr<TextureAtlas> getAtlas();
private:
	int _symbolHeight;
	std::shared_ptr<TextureAtlas> _charactersAtlas;
	std::map<GLchar, std::tuple<float, float> > _charactersAtlasPosition;
	std::map<GLchar, FT_Face> _characters;
};


