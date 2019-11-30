#pragma once
#include <memory>
#include <string>
#include <tuple>

#include "ImageLoader.h"

class TextureRaw {
private:
	std::string _path;
	//rows and cols number
	std::tuple<float, float> _tileSize;
	int _textureID;
	std::shared_ptr<ImageLoader> _imageLoader;
public:
	TextureRaw(std::string path, std::tuple<float, float> tileSize);
	std::shared_ptr<ImageLoader> getImageLoader();
	bool setTextureID(int textureID);
	int getTextureID();
	std::tuple<float, float> getTileSize();
};