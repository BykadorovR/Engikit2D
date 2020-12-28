#include "ImageLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLenum ImageLoader::getGLColorFormat() {
	switch (_channels) {
	case 1:
		return GL_LUMINANCE;
	case 3:
		return GL_RGB;
	case 4:
		return GL_RGBA;
	}

	return 0;
}

//Load PNG file to visualise as Texture
int ImageLoader::loadPNG(const char* relativePath) {
	assert(relativePath != NULL);
	//force load in RGBA format
	_channels = 4;
	unsigned char *src = stbi_load(relativePath, &_width, &_height, nullptr, _channels);
	_data = std::vector<uint8_t>(src, src + _width * _height * _channels);

	return false;
}

std::tuple<float, float> ImageLoader::getSize() {
	return { _width, _height };
}

std::vector<unsigned char> ImageLoader::getData() {
	return _data;
}

