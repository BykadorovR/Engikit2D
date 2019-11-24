#pragma once
#include <GL/glew.h>
#include <vector>
#include <tuple>
#include <memory>

#include "Texture.h"

class TextureAtlas {
private:
	std::tuple<float, float> _size;
	std::vector<uint8_t> _data;
	GLuint _textureObjectId;
	int _atlasID;
	std::vector < std::shared_ptr<Texture> > _textures;
public:
	TextureAtlas(std::tuple<float, float> size);
	bool addTexture(std::shared_ptr<Texture> texture, std::tuple<int, int> position);
	std::shared_ptr<Texture> getTexture(int textureID);
	std::vector<std::shared_ptr<Texture> > getTextureList();
};