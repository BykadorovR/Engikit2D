#pragma once
#include <GL/glew.h>
#include <vector>
#include <tuple>
#include <memory>

#include "Texture.h"

class TextureAtlas {
public:
	TextureAtlas(GLenum fourCC, std::tuple<float, float> size);
	bool initialize();
	bool addTexture(std::shared_ptr<TextureRaw> texture, std::tuple<float, float> position);
	std::tuple<std::shared_ptr<TextureRaw>, std::tuple<float, float> > getTexture(int textureID);
	bool containTexture(int textureID);
	int getAtlasID();
	bool setAtlasID(int atlasID);
	GLuint getTextureObject();
	std::tuple<float, float> getSize();
private:
	std::tuple<float, float> _size;
	std::vector<uint8_t> _data;
	GLuint _textureObjectId;
	int _atlasID;
	int _textureCounter = 0;
	GLenum _fourCC;
	int _bitDepth;
	//Texture and position of this texture in the current atlas
	std::vector<std::tuple<std::shared_ptr<TextureRaw>, std::tuple<float, float> > > _textures;
};