#pragma once
#include <memory>
#include "TextureAtlas.h"

class TextureManager {
private:
	std::vector<std::shared_ptr<TextureAtlas> > _atlasList;
	int _textureAtlasCounter = 0;
public:
	static TextureManager* instance() {
		static TextureManager textureManager;
		return &textureManager;
	}
	void printTextures();
	std::shared_ptr<TextureRaw> createTexture(std::string imagePath, int atlasID, std::tuple<float, float> posAtlas, std::tuple<float, float> tileSize);
	std::shared_ptr<TextureRaw> createTexture(int atlasID, std::tuple<float, float> posAtlas, std::vector<uint8_t> data, std::tuple<float, float> realImageSize);
	std::shared_ptr<TextureAtlas> getTextureAtlas(int textureID);
	std::shared_ptr<TextureAtlas> createAtlas(GLenum fourCC, std::tuple<float, float> size);
};