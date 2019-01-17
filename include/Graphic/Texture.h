#pragma once
#include "platformGL.h"
#include <vector>
#include <string>


using namespace std;

class Texture {
public:
	Texture(int width, int height);
	Texture(string path);

	GLuint loadTexture();
	void addTexture(Texture image, int start_x, int start_y);

	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}

	unsigned char* getData() {
		return &data[0];
	}

	unsigned char& operator[](int index) {
		return data[index];
	}
protected:
	int width, height;
	std::vector<unsigned char> data;
};