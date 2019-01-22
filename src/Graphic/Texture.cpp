#include "Texture.h"
#include "Image.h"
#include <assert.h>

#define DEPTH_COLOR 4

Texture::Texture(string path) {
	ImageLoader loader;
	loader.loadPNG(&path[0]);
	_width = loader.getWidth();
	_height = loader.getHeight();
	_data = loader.getData();
}

Texture::Texture(int width, int height) {
	_width = width;
	_height = height;
	_data = std::vector<unsigned char>(width * height * DEPTH_COLOR);
}

Texture::Texture() {
}

void Texture::addTexture(Texture& image, int startX, int startY) {
	//here we hardcode RGBA format using 4 byte to represent 1 color
	for (int y = 0; y < image.getHeight(); y++)
		for (int x = 0; x < image.getWidth() * DEPTH_COLOR; x++) {
			int coordXIntoResult = startX * DEPTH_COLOR + x;
			int coordYIntoResult = startY + y;
			assert(coordXIntoResult < _width * DEPTH_COLOR);
			assert(coordYIntoResult < _height);
			int coordIntoResult = coordXIntoResult + _width * DEPTH_COLOR * coordYIntoResult;
			int coordIntoSource = x + image.getWidth() * DEPTH_COLOR * y;
			assert(coordIntoResult < _width * _height * DEPTH_COLOR);
			assert(coordIntoSource < image.getHeight() * image.getWidth() * DEPTH_COLOR);
			_data[coordIntoResult] = image[coordIntoSource];
		}

	image.setParent(std::shared_ptr<ParentTexture>(new ParentTexture(this, std::pair<int, int>(startX, startY))));
}

//should be platform specific
GLuint Texture::loadTexture() {
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
	return textureObjectId;
}