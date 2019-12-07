#pragma once
#include <memory>
#include <string>
#include <tuple>

#include "ImageLoader.h"

class TextureRaw {
private:
	std::vector<uint8_t> _data;
	//rows and cols number
	std::tuple<float, float> _tileSize;
	//real size of picture
	std::tuple<float, float> _realImageSize;
	int _textureID;
public:
	TextureRaw(std::string path, std::tuple<float, float> tileSize);
	TextureRaw(std::vector<uint8_t> data, std::tuple<float, float> realImageSize);
	bool setTextureID(int textureID);
	int getTextureID();
	std::tuple<float, float> getTileSize();
	std::tuple<float, float> getRealImageSize();
	std::vector<uint8_t> getData();

};