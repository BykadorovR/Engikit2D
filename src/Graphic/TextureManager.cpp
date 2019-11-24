#include "TextureManager.h"

std::shared_ptr<TextureAtlas> findAtlas(std::vector<std::shared_ptr<TextureAtlas> > atlasList, int atlasID) {
	std::shared_ptr<TextureAtlas> targetAtlas = nullptr;
	for (auto atlas : atlasList) {
		if (atlasID == atlas->getAtlasID()) {
			//we found needed atlas
			targetAtlas = atlas;
		}
	}

	return targetAtlas;
}

std::shared_ptr<TextureRaw> TextureManager::createTexture(std::string imagePath, int atlasID, std::tuple<float, float> posAtlas, std::tuple<float, float> tileSize) {
	std::shared_ptr<TextureAtlas> targetAtlas = findAtlas(_atlasList, atlasID);
	std::shared_ptr<TextureRaw> textureRaw = std::make_shared<TextureRaw>(imagePath, tileSize);
	targetAtlas->addTexture(textureRaw, posAtlas);
	return textureRaw;
}

std::shared_ptr<TextureRaw> TextureManager::getTexture(int textureID) {
	std::shared_ptr<TextureRaw> texture = nullptr;
	for (auto atlas : _atlasList) {
		texture = atlas->getTexture(textureID);
	}
	return texture;
}

std::shared_ptr<TextureAtlas> TextureManager::createAtlas(std::tuple<float, float> size) {
	std::shared_ptr<TextureAtlas> targetAtlas = std::make_shared<TextureAtlas>(size);
	targetAtlas->setAtlasID(_textureAtlasCounter++);
	_atlasList.push_back(targetAtlas);
	return targetAtlas;
}