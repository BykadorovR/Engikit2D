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

void TextureAtlas::initializeAtlas() {
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
	_textureObjectId = textureObjectId;
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

int TextureAtlas::getAtlasID() {
	return _atlasID;
}

void TextureAtlas::setAtlasID(int atlasID) {
	_atlasID = atlasID;
}

GLuint TextureAtlas::getTexureObjectID() {
	return _textureObjectId;
}

Texture::Texture() {

}

Texture::Texture(std::string path, int posXAtlas, int posYAtlas, std::shared_ptr<TextureAtlas> atlas) {
	_path = path;
	_posXAtlas = posXAtlas;
	_posYAtlas = posYAtlas;
	_atlas = atlas;
	_row = 0;
	_column = 0;
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

int Texture::getTextureID() {
	return _textureID;
}

void Texture::setTextureID(int textureID) {
	_textureID = textureID;
}

std::shared_ptr<Texture> TextureManager::getTexture(int textureID) {
	textureList[textureID]->getAtlas()->initializeAtlas();
	return textureList[textureID];
}

std::vector<std::shared_ptr<Texture> > TextureManager::getTextureList() {
	return textureList;
}

std::vector<std::shared_ptr<TextureAtlas> > TextureManager::getAtlasList() {
	return atlasList;
}

void TextureManager::printTextures() {
	for (int i = 0; i < textureList.size(); i++) {
		std::cout << "Path: " << textureList[i]->getPath() << ";\n ID: " << textureList[i]->getTextureID() << "; Size: " << textureList[i]->getWidth() << "x" << textureList[i]->getHeight()
				  << "; Atlas ID: " << textureList[i]->getAtlas()->getAtlasID() << "; Atlas position: " 
			      << textureList[i]->getPosXAtlas() << "x" << textureList[i]->getPosYAtlas() << std::endl;
	}
	if (textureList.size() == 0)
		std::cout << "Empty list" << std::endl;
}

std::shared_ptr<TextureAtlas> TextureManager::loadAtlas(int atlasID, int width, int height) {
	std::shared_ptr<TextureAtlas> targetAtlas = std::make_shared<TextureAtlas>(width, height);
	targetAtlas->setAtlasID(atlasID);
	atlasList.push_back(targetAtlas);
	return targetAtlas;
}

std::shared_ptr<TextureAtlas> TextureManager::findAtlas(int atlasID) {
	std::shared_ptr<TextureAtlas> targetAtlas = nullptr;
	for (auto atlas : atlasList) {
		if (atlasID == atlas->getAtlasID()) {
			//we found needed atlas
			targetAtlas = atlas;
		}
	}
	//atlas wasn't found need to create the new one
	if (targetAtlas == nullptr) {
		targetAtlas = loadAtlas(atlasID, 4096, 4096);
	}
	return targetAtlas;
}
 
std::shared_ptr<Texture> TextureManager::loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY) {
	std::shared_ptr<TextureAtlas> targetAtlas = findAtlas(atlasID);
	std::shared_ptr<Texture> textureRaw = std::make_shared<Texture>(imagePath, atlasX, atlasY, targetAtlas);
	textureRaw->setTextureID(textureCounter++);
	textureList.push_back(textureRaw);
	return textureRaw;
}

std::shared_ptr<Texture> TextureManager::loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY, int tileX, int tileY) {
	std::shared_ptr<TextureAtlas> targetAtlas = findAtlas(atlasID);
	std::shared_ptr<Texture> textureRaw = std::make_shared<Texture>(imagePath, atlasX, atlasY, tileX, tileY, targetAtlas);
	textureRaw->setTextureID(textureCounter++);
	textureList.push_back(textureRaw);
	return textureRaw;
}