#include "Texture.h"
#include <assert.h>

#pragma once
#include "platformGL.h"
#include <vector>
#include <string>
#include <assert.h>
#include "Shader.h"
#include <iostream>

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
	_path = path;
	_posXAtlas = posXAtlas;
	_posYAtlas = posYAtlas;
	_atlas = atlas;
	//
	_imageLoader = std::shared_ptr<ImageLoader>(new ImageLoader);
	_imageLoader->loadPNG(&path[0]);
	_width = _imageLoader->getWidth();
	_height = _imageLoader->getHeight();
	_atlas->addTexture(_imageLoader->getData(), _posXAtlas, _posYAtlas, _width, _height);
}

Texture::Texture(std::string path, int posXAtlas, int posYAtlas, int row, int column, std::shared_ptr<TextureAtlas> atlas) 
	    : Texture(path, posXAtlas, posYAtlas, atlas) {
	_row = row;
	_column = column;
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

int Texture::getColumn() {
	return _column;
}
int Texture::getRow() {
	return _row;
}

int Texture::getPosXAtlas() {
	return _posXAtlas;
}

int Texture::getPosYAtlas() {
	return _posYAtlas;
}

std::string Texture::getPath() {
	return _path;
}

std::shared_ptr<Texture> TextureManager::getTexture(int textureID) {
	return textureList[textureID];
}

void TextureManager::printTextures() {
	for (int i = 0; i < textureList.size(); i++) {
		std::cout << "Path: " << textureList[i]->getPath() << ";\n ID: " << i << "; Size: " << textureList[i]->getWidth() << "x" << textureList[i]->getHeight()
				  << "; Atlas ID: " << findAtlasID(textureList[i]->getAtlas()) << "; Atlas position: " 
			      << textureList[i]->getPosXAtlas() << "x" << textureList[i]->getPosYAtlas() << std::endl;
	}
}

int TextureManager::findAtlasID(std::shared_ptr<TextureAtlas> atlas) {
	for (map<int, std::shared_ptr<TextureAtlas> >::iterator it = atlasMap.begin(); it != atlasMap.end(); ++it) {
		if (atlas == it->second) {
			//we found needed atlas
			return it->first;
		}
	}
}

std::shared_ptr<TextureAtlas> TextureManager::findAtlas(int atlasID) {
	std::shared_ptr<TextureAtlas> targetAtlas = nullptr;
	for (map<int, std::shared_ptr<TextureAtlas> >::iterator it = atlasMap.begin(); it != atlasMap.end(); ++it) {
		if (atlasID == it->first) {
			//we found needed atlas
			targetAtlas = it->second;
		}
	}
	//atlas wasn't found need to create the new one
	if (targetAtlas == nullptr) {
		targetAtlas = std::make_shared<TextureAtlas>(4096, 4096);
		atlasMap[atlasID] = targetAtlas;
		targetAtlas->loadAtlas();
	}
	return targetAtlas;
}
 
void TextureManager::loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY) {
	std::shared_ptr<TextureAtlas> targetAtlas = findAtlas(atlasID);
	std::shared_ptr<Texture> textureRaw = std::make_shared<Texture>(imagePath, atlasX, atlasY, targetAtlas);
	textureList.push_back(textureRaw);
}

void TextureManager::loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY, int tileX, int tileY) {
	std::shared_ptr<TextureAtlas> targetAtlas = findAtlas(atlasID);
	std::shared_ptr<Texture> textureRaw = std::make_shared<Texture>(imagePath, atlasX, atlasY, tileX, tileY, targetAtlas);
	textureList.push_back(textureRaw);
}