#include "GlyphsLoader.h"
#include <algorithm>
#include "Texture.h"

GlyphsLoader::GlyphsLoader() {
	//Need to create atlas without TextureManager so it can't be used outside
	_charactersAtlas = std::make_shared<TextureAtlas>(GL_RGBA, std::tuple<float, float>(1024, 1024));
}

std::shared_ptr<TextureAtlas> GlyphsLoader::getAtlas() {
	return _charactersAtlas;
}

std::map<GLchar, CharacterInfo> GlyphsLoader::getCharacters() {
	return _characters;
}

std::map<GLchar, std::tuple<float, float> > GlyphsLoader::getCharactersAtlasPosition() {
	return _charactersAtlasPosition;
}

void GlyphsLoader::bufferSymbols(int symbolHeight) {
	_symbolHeight = symbolHeight;

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "../data/fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	FT_Set_Pixel_Sizes(face, 0, symbolHeight);
	float maxHeight = 0;
	float atlasX = 0;
	float atlasY = 0;
	for (GLubyte c = 48; c < 122; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		std::tuple<float, float> charSize = { face->glyph->bitmap.width, face->glyph->bitmap.rows };
		maxHeight = std::max(std::get<1>(charSize), maxHeight);
		if (atlasX + std::get<0>(charSize) >= std::get<0>(_charactersAtlas->getSize())) {
			atlasY += maxHeight + 1; //+ 1 = to delimit chars by height
			atlasX = 0;
		}
		std::tuple<float, float> atlasPosition = { atlasX, atlasY };

		atlasX += std::get<0>(charSize) + 1; //+ 1 = to delimit chars by width
		std::vector<uint8_t> charData(face->glyph->bitmap.buffer, face->glyph->bitmap.buffer +
			static_cast<int>(std::get<0>(charSize) * std::get<1>(charSize)));

		//Need to create texture without TextureManager so it can't be used outside
		std::shared_ptr<TextureRaw> glyph = std::make_shared<TextureRaw>(charData, charSize);
		_charactersAtlas->addTextureFont(glyph, atlasPosition);
		//bind position of character in atlas to character so texture rendering will depends on only 1 texture and position
		//inside it
		_charactersAtlasPosition.insert(std::pair<GLchar, std::tuple<float, float> >(c, atlasPosition));

		CharacterInfo info = { 
			std::tuple<int, int>(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			std::tuple<int, int>(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x };

		_characters.insert(std::pair<GLchar, CharacterInfo>(c, info));
	}
	_charactersAtlas->initialize();
}