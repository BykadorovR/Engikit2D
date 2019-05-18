#pragma once
#include "platformGL.h"
#include <vector>
#include <string>
#include <assert.h>
#include "Image.h"
#include <map>

using namespace std;

#define DEPTH_COLOR 4

class TextureAtlas {
public:
	TextureAtlas();
	TextureAtlas(int width, int height);

	void addTexture(std::vector<unsigned char> data, int startX, int startY, int width, int height);
	void initializeAtlas();
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

	std::shared_ptr<TextureAtlas> getAtlas();
	int getWidth();
	int getHeight();
	int getPosXAtlas();
	int getPosYAtlas();
	int getColumn();
	int getRow();
	int getX();
	int getY();
	std::string getPath();

private:
	std::shared_ptr<TextureAtlas> _atlas;
	std::shared_ptr<ImageLoader> _imageLoader;
	std::string _path;
	int _posXAtlas, _posYAtlas;
	int _width, _height;
	int _row, _column;
};

class TextureManager {
public:
	TextureManager() {

	}
	void printTextures();
	void loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY);
	void loadTexture(std::string imagePath, int atlasID, int atlasX, int atlasY, int tileX, int tileY);
	std::shared_ptr<Texture> getTexture(int textureID);
private:
	int findAtlasID(std::shared_ptr<TextureAtlas> atlas);
	std::shared_ptr<TextureAtlas> findAtlas(int atlasID);
	std::map<int, std::shared_ptr<TextureAtlas> > atlasMap;
	std::vector<std::shared_ptr<Texture> > textureList;
};