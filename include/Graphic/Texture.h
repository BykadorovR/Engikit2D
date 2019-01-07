#pragma once
#include "platformGL.h"
#include <vector>
#include <string>


using namespace std;

class Texture {
	int width, height;
	vector<unsigned char> data;
	const int depth = 4;
public:
	Texture(int _width, int _height) : width(_width), height(_height) {
		data.resize(width * height * depth);
	}

	Texture(string path);

	~Texture() {
		data.clear();
	}
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
};