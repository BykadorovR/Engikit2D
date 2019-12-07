#include "GlyphsLoader.h"
#include <algorithm>
#include "Texture.h"

GlyphsLoader::GlyphsLoader() {
	//Need to create atlas without TextureManager so it can't be used outside
	_charactersAtlas = std::make_shared<TextureAtlas>(GL_RED, std::tuple<float, float>(4096, 4096));
}

std::shared_ptr<TextureAtlas> GlyphsLoader::getAtlas() {
	return _charactersAtlas;
}

std::map<GLchar, FT_Face> GlyphsLoader::getCharacters() {
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		
		_characters.insert(std::pair<GLchar, FT_Face>(c, face));
	}

	//Load all character's textures to texture atlas created above
	float maxHeight = 0;
	float atlasX = 0;
	float atlasY = 0;
	for (auto item : _characters) {
		std::tuple<float, float> charSize = { item.second->glyph->bitmap.width, item.second->glyph->bitmap.rows };
		std::tuple<float, float> atlasPosition = { atlasX, atlasY };
		maxHeight = std::max(std::get<1>(charSize), maxHeight);
		if (atlasX * item.first >= std::get<0>(_charactersAtlas->getSize()))
			atlasY += maxHeight;

		atlasX += std::get<0>(charSize);
		std::vector<uint8_t> charData(item.second->glyph->bitmap.buffer, item.second->glyph->bitmap.buffer +
			static_cast<int>(std::get<0>(charSize) * std::get<1>(charSize)));
		//Need to create texture without TextureManager so it can't be used outside
		std::shared_ptr<TextureRaw> glyph = std::make_shared<TextureRaw>(charData, charSize);
		_charactersAtlas->addTexture(glyph, atlasPosition);
		//bind position of character in atlas to character so texture rendering will depends on only 1 texture and position
		//inside it
		_charactersAtlasPosition.insert(std::pair<GLchar, std::tuple<float, float> >(std::get<0>(item), atlasPosition));
	}
	_charactersAtlas->initialize();
}