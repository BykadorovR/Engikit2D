#include <assert.h>
#include <algorithm>

#include "TextureAtlas.h"

#define DEPTH_COLOR 4

TextureAtlas::TextureAtlas(std::tuple<float, float> size) {
	_size = size;
	_data.resize(std::get<0>(size) * std::get<1>(size) * DEPTH_COLOR);

	GLuint textureObjectId;
	GLenum ARGBFormat = 0x1908;
	glGenTextures(1, &textureObjectId);
	assert(textureObjectId != 0);
	glBindTexture(GL_TEXTURE_2D, textureObjectId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, ARGBFormat, std::get<0>(size), std::get<1>(size), 0, ARGBFormat, GL_UNSIGNED_BYTE, &_data[0]);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	_textureObjectId = textureObjectId;
}

bool TextureAtlas::addTexture(std::shared_ptr<TextureRaw> texture, std::tuple<float, float> position) {
	for (int y = 0; y < std::get<1>(_size); y++)
		for (int x = 0; x < std::get<0>(_size) * DEPTH_COLOR; x++) {
			int coordXIntoResult = std::get<0>(position) * DEPTH_COLOR + x;
			int coordYIntoResult = std::get<1>(position) + y;
			assert(coordXIntoResult < std::get<0>(_size) * DEPTH_COLOR);
			assert(coordYIntoResult < std::get<1>(_size));
			int coordIntoResult = coordXIntoResult + std::get<0>(_size) * DEPTH_COLOR * coordYIntoResult;
			int coordIntoSource = x + std::get<0>(_size) * DEPTH_COLOR * y;
			assert(coordIntoResult < std::get<0>(_size) * std::get<1>(_size) * DEPTH_COLOR);
			assert(coordIntoSource < std::get<0>(_size) * std::get<1>(_size) * DEPTH_COLOR);
			_data[coordIntoResult] = texture->getImageLoader()->getData()[coordIntoSource];
		}

	texture->setTextureID(_textureCounter++);

	return true;
}

int TextureAtlas::getAtlasID() {
	return _atlasID;
}

bool TextureAtlas::setAtlasID(int atlasID) {
	_atlasID = atlasID;
	return false;
}

std::tuple<std::shared_ptr<TextureRaw>, std::tuple<float, float> > TextureAtlas::getTexture(int textureID) {
	auto textureIterator = std::find_if(_textures.begin(), _textures.end(), [textureID](std::tuple< std::shared_ptr<TextureRaw>, std::tuple<float, float> > texture) {return std::get<0>(texture)->getTextureID() == textureID; });
	return *textureIterator;
}

bool TextureAtlas::containTexture(int textureID) {
	auto textureIterator = std::find_if(_textures.begin(), _textures.end(), [textureID](std::tuple< std::shared_ptr<TextureRaw>, std::tuple<float, float> > texture) {return std::get<0>(texture)->getTextureID() == textureID; });
	if (textureIterator != _textures.end())
		return true;
	return false;
}

GLuint TextureAtlas::getTextureObject() {
	return _textureObjectId;
}