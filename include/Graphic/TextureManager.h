#pragma once
#include <memory>

class TextureManager {
private:
	TextureManager();
public:
	static TextureManager* instance() {
		static TextureManager textureManager;
		return &textureManager;
	}
	void printTextures();
	std::shared_ptr<Texture> loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY);
	std::shared_ptr<Texture> loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY, int tileX, int tileY);
	std::shared_ptr<Texture> getTexture(int textureID);
	std::vector<std::shared_ptr<Texture> > getTextureList();
	std::vector<std::shared_ptr<TextureAtlas> > getAtlasList();
	std::shared_ptr<TextureAtlas> loadAtlas(int atlasID, int width, int height);
private:
	std::shared_ptr<TextureAtlas> findAtlas(int atlasID);
	std::vector<std::shared_ptr<TextureAtlas> > atlasList;
	std::vector<std::shared_ptr<Texture> > textureList;
};