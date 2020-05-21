#include "GlyphsLoader.h"
#include <algorithm>
#include "Texture.h"

GlyphsLoader& GlyphsLoader::instance() {
	static GlyphsLoader object;
	return object;
}

void GlyphsLoader::initialize(std::string fontPath, std::tuple<int, int> symbolsCodes) {
	//Need to create atlas without TextureManager so it can't be used outside
	_charactersAtlas = std::make_shared<TextureAtlas>(GL_RGBA, std::tuple<float, float>(256, 256));
	_symbolsCodes = symbolsCodes;
	_fontPath = fontPath;
}

std::shared_ptr<TextureAtlas> GlyphsLoader::getAtlas() {
	return _charactersAtlas;
}

float GlyphsLoader::getLineSpace() {
	return _lineSpace;
}

float GlyphsLoader::getGlyphHeight() {
	return _symbolHeight;
}

std::map<FT_ULong, CharacterInfo> GlyphsLoader::getCharacters() {
	return _characters;
}

std::map<FT_ULong, std::tuple<float, float> > GlyphsLoader::getCharactersAtlasPosition() {
	return _charactersAtlasPosition;
}

void GlyphsLoader::bufferSymbols(int symbolHeight) {
	_symbolHeight = symbolHeight;

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, _fontPath.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	
	FT_Select_Charmap(face, ft_encoding_unicode);

	FT_Set_Pixel_Sizes(face, 0, symbolHeight);
	// get the line spacing, note that it is measured in 64ths of a pixel
	_lineSpace = face->size->metrics.height / 64;

	float maxHeight = 0;
	float atlasX = 0;
	float atlasY = 0;

	std::vector<int> codes;
	//from " " to "~" including all digits and eng alphabet 
	for (int i = 20; i <= 126; i++) {
		codes.push_back(i);
	}

	for (int i = std::get<0>(_symbolsCodes); i <= std::get<1>(_symbolsCodes); i++) {
		codes.push_back(i);
	}
	for (auto c : codes)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		auto glyph_index = FT_Get_Char_Index(face, c);

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
		_charactersAtlasPosition.insert(std::pair<FT_ULong, std::tuple<float, float> >(c, atlasPosition));

		CharacterInfo info = { 
			std::tuple<int, int>(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			std::tuple<int, int>(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x };

		_characters.insert(std::pair<FT_ULong, CharacterInfo>(c, info));
	}
	_charactersAtlas->initialize();
}