#include "Texture.h"

TextureRaw::TextureRaw(std::string path, std::tuple<float, float> tileSize) {
	_path = path;
	_tileSize = tileSize;
	_imageLoader = std::shared_ptr<ImageLoader>(new ImageLoader);
	_imageLoader->loadPNG(&path[0]);
}

std::shared_ptr<ImageLoader> TextureRaw::getImageLoader() {
	return _imageLoader;
}

int TextureRaw::getTextureID() {
	return _textureID;
}

bool TextureRaw::setTextureID(int textureID) {
	_textureID = textureID;
	return false;
}