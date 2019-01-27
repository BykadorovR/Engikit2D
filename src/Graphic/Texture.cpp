#include "Texture.h"
#include "Image.h"
#include <assert.h>

#pragma once
#include "platformGL.h"
#include <vector>
#include <string>
#include <assert.h>

using namespace std;

#define DEPTH_COLOR 4

TextureAtlas::TextureAtlas() {
}

TextureAtlas::TextureAtlas(int width, int height) {
	_width = width;
	_height = height;
	_data.resize(_width * _height * DEPTH_COLOR);
}

void TextureAtlas::addTexture(std::vector<unsigned char> data, int startX, int startY, int width, int height) {
	for (int y = 0; y < height; y++)
		for (int x = 0; x < width * DEPTH_COLOR; x++) {
			int coordXIntoResult = startX * DEPTH_COLOR + x;
			int coordYIntoResult = startY + y;
			assert(coordXIntoResult < _width * DEPTH_COLOR);
			assert(coordYIntoResult < _height);
			int coordIntoResult = coordXIntoResult + _width * DEPTH_COLOR * coordYIntoResult;
			int coordIntoSource = x + width * DEPTH_COLOR * y;
			assert(coordIntoResult < _width * _height * DEPTH_COLOR);
			assert(coordIntoSource < width * height * DEPTH_COLOR);
			_data[coordIntoResult] = data[coordIntoSource];
		}
}

void TextureAtlas::loadAtlas() {
	GLuint textureObjectId;
	GLenum ARGBFormat = 0x1908;
	glGenTextures(1, &textureObjectId);
	assert(textureObjectId != 0);
	glBindTexture(GL_TEXTURE_2D, textureObjectId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, ARGBFormat, _width, _height, 0, ARGBFormat, GL_UNSIGNED_BYTE, &_data[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	_atlasID = textureObjectId;
}

int TextureAtlas::getWidth() {
	return _width;
}

int TextureAtlas::getHeight() {
	return _height;
}

unsigned char* TextureAtlas::getData() {
	return &_data[0];
}

unsigned char& TextureAtlas::operator[](int index) {
	return _data[index];
}

GLuint TextureAtlas::getAtlasID() {
	return _atlasID;
}

Texture::Texture() {

}

Texture::Texture(std::string path, int posXAtlas, int posYAtlas, std::shared_ptr<TextureAtlas> atlas) {
	_posXAtlas = posXAtlas;
	_posYAtlas = posYAtlas;
	_atlas = atlas;
	//
	ImageLoader loader;
	loader.loadPNG(&path[0]);
	_width = loader.getWidth();
	_height = loader.getHeight();
	_atlas->addTexture(loader.getData(), _posXAtlas, _posYAtlas, _width, _height);
}

int Texture::getWidth() {
	return _width;
}

int Texture::getHeight() {
	return _height;
}

std::shared_ptr<TextureAtlas> Texture::getAtlas() {
	return _atlas;
}

int Texture::getX() {
	return _posXAtlas;
}
int Texture::getY() {
	return _posYAtlas;
}
