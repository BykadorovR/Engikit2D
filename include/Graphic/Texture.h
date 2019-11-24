#pragma once
#include <memory>
#include <string>
#include <tuple>

#include "ImageLoader.h"

class Texture {
private:
	std::string _path;
	std::tuple<float, float> _size;
	std::tuple<float, float> _tileSize;
	int _textureID;
	std::shared_ptr<ImageLoader> _imageLoader;
public:
	
};