#pragma once
#include "platformGL.h"
#include <vector>
#include <string>


using namespace std;

class Texture {
	ImageLoader loader;
public:

	Texture(string path);

	void joinTextures(Texture image, int start_x, int start_y, Texture& result_image);
	GLuint loadTexture(const GLsizei width, const GLsizei height, const GLenum type, const GLvoid* pixels);
	GLuint loadRawImageToTexture(Texture& rawImageData);

	int getWidth() {
		return loader.getWidth();
	}

	int getHeight() {
		return loader.getHeight();
	}

	unsigned char* getData() {
		return &loader.getData()[0];
	}

	unsigned char& operator[](int index) {
		return loader.getData()[index];
	}
};