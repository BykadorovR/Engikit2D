#pragma once
#include "platformGL.h"
#include <vector>
#include <string>
#include <assert.h>
#include "Image.h"

using namespace std;

#define DEPTH_COLOR 4

class TextureAtlas {
public:
	TextureAtlas();
	TextureAtlas(int width, int height);

	void addTexture(std::vector<unsigned char> data, int startX, int startY, int width, int height);
	void loadAtlas();

	int getWidth();
	int getHeight();
	unsigned char* getData();
	unsigned char& operator[](int index);
	GLuint getAtlasID();

private:
	int _width, _height;
	std::vector<unsigned char> _data;
	GLuint _atlasID;
};

class Texture {
public:
	Texture();
	Texture(std::string path, int posXAtlas, int posYAtlas, std::shared_ptr<TextureAtlas> atlas);
	Texture(std::string path, int posXAtlas, int posYAtlas, int row, int column, std::shared_ptr<TextureAtlas> atlas);
	Texture(std::string path, int posXAtlas, int posYAtlas);
	Texture(std::string path, int posXAtlas, int posYAtlas, int row, int column);

	std::shared_ptr<TextureAtlas> getAtlas();
	void setAtlas(std::shared_ptr<TextureAtlas> atlas);
	int getWidth();
	int getHeight();
	int getColumn();
	int getRow();
	int getX();
	int getY();

private:
	std::shared_ptr<TextureAtlas> _atlas;
	std::shared_ptr<ImageLoader> _imageLoader;
	int _posXAtlas, _posYAtlas;
	int _width, _height;
	int _row, _column;
};
