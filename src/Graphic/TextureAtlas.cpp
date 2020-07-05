#include <algorithm>
#include <cassert>
#include "TextureAtlas.h"

TextureAtlas::TextureAtlas(GLenum fourCC, std::tuple<float, float> size) {
	_size = size;
	_fourCC = fourCC;
	_bitDepth = 1;
	if (_fourCC == GL_RGBA)
		_bitDepth = 4;
	glGenTextures(1, &_textureObjectId);
	assert(_textureObjectId != 0);

	_data.resize(std::get<0>(_size) * std::get<1>(_size) * _bitDepth);
}

bool TextureAtlas::initialize() {
	glBindTexture(GL_TEXTURE_2D, _textureObjectId);
	// Set texture options
	glTexImage2D(GL_TEXTURE_2D, 0, _fourCC, std::get<0>(_size), std::get<1>(_size), 0, _fourCC, GL_UNSIGNED_BYTE, &_data[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	return false;
}

bool TextureAtlas::addTexture(std::shared_ptr<TextureRaw> texture, std::tuple<float, float> position) {
	auto textureSize = texture->getRealImageSize();
	auto textureData = texture->getData();
	for (int y = 0; y < std::get<1>(textureSize); y++)
		for (int x = 0; x < std::get<0>(textureSize) * _bitDepth; x++) {
			int coordXIntoResult = std::get<0>(position) * _bitDepth + x;
			int coordYIntoResult = std::get<1>(position) + y;
			assert(coordXIntoResult < std::get<0>(_size) * _bitDepth);
			assert(coordYIntoResult < std::get<1>(_size));
			int coordIntoResult = coordXIntoResult + std::get<0>(_size) * _bitDepth * coordYIntoResult;
			int coordIntoSource = x + std::get<0>(textureSize) * _bitDepth * y;
			assert(coordIntoResult < std::get<0>(_size) * std::get<1>(_size) * _bitDepth);
			assert(coordIntoSource < std::get<0>(textureSize) * std::get<1>(textureSize) * _bitDepth);
			_data[coordIntoResult] = textureData[coordIntoSource];
		}

	_textures.push_back({texture, position});
	texture->setTextureID(_textureCounter++);

	return false;
}

bool TextureAtlas::addTextureFont(std::shared_ptr<TextureRaw> texture, std::tuple<float, float> position) {
	auto textureSize = texture->getRealImageSize();
	auto textureData = texture->getData();
	for (int y = 0; y < std::get<1>(textureSize); y++)
		for (int x = 0; x < std::get<0>(textureSize); x++) {
			int coordXIntoResult = std::get<0>(position) * _bitDepth + x * _bitDepth;
			int coordYIntoResult = std::get<1>(position) + y;
			assert(coordXIntoResult < std::get<0>(_size) * _bitDepth);
			assert(coordYIntoResult < std::get<1>(_size));
			int coordIntoResult = coordXIntoResult + std::get<0>(_size) * _bitDepth * coordYIntoResult;
			int coordIntoSource = x + std::get<0>(textureSize) * y;
			assert(coordIntoResult < std::get<0>(_size) * std::get<1>(_size) * _bitDepth);
			assert(coordIntoSource < std::get<0>(textureSize) * std::get<1>(textureSize));
			for (int k = 0; k < 3; k++)
				_data[coordIntoResult + k] = 255;
			_data[coordIntoResult + _bitDepth - 1] = textureData[coordIntoSource];
		}

	_textures.push_back({ texture, position });
	texture->setTextureID(_textureCounter++);

	return false;
}


int TextureAtlas::getAtlasID() {
	return _textureObjectId;
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

std::tuple<float, float> TextureAtlas::getSize() {
	return _size;
}