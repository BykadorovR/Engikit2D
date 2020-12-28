#pragma once
#include <GL/glew.h>
#include <vector>
#include <tuple>

class ImageLoader {
public:
	//return status
	int loadPNG(const char* relativePath);
	GLenum getGLColorFormat();
	std::tuple<float, float> getSize();
	std::vector<unsigned char> getData();
private:
	int _width;
	int _height;
	int _channels;
	std::vector<uint8_t> _data;
};