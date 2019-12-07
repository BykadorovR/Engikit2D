#include "Texture.h"

TextureRaw::TextureRaw(std::string path, std::tuple<float, float> tileSize) {
	_tileSize = tileSize;
	auto imageLoader = std::shared_ptr<ImageLoader>(new ImageLoader);
	imageLoader->loadPNG(&path[0]);

	_data = imageLoader->getData();
	_realImageSize = imageLoader->getSize();
}

TextureRaw::TextureRaw(std::vector<uint8_t> data, std::tuple<float, float> realImageSize) {
	_tileSize = { 1.0f, 1.0f };
	_data = data;
	_realImageSize = realImageSize;
}

std::tuple<float, float> TextureRaw::getRealImageSize() {
	return _realImageSize;
}

std::vector<uint8_t> TextureRaw::getData() {
	return _data;
}

int TextureRaw::getTextureID() {
	return _textureID;
}

bool TextureRaw::setTextureID(int textureID) {
	_textureID = textureID;
	return false;
}

std::tuple<float, float> TextureRaw::getTileSize() {
	return _tileSize;
}