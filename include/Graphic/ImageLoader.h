#pragma once

#include <GL/glew.h>
#include <png.h>
#include <vector>
#include <tuple>

//needed for internal libPNG conversions
struct DataHandle {
	DataHandle(png_byte* _data, png_size_t _size, png_size_t _offset) : data(_data), size(_size), offset(_offset) {

	}
	png_byte* data;
	png_size_t size;
	png_size_t offset;
};

//needed for internal libPNG function
void PNGCallback(png_structp PNGPtr, png_byte* rawData, png_size_t readLength);

class ImageLoader {
public:
	//return status
	int loadPNG(const char* relativePath);
	GLenum getGLColorFormat();
	std::tuple<float, float> getSize();
	int getBitdepth();
	std::vector<unsigned char> getData();

private:
	int readAndUpdateInfo(png_structp PNGPtr, png_infop infoPTR);
	int getImageFromPNG(std::string PNGData);
	int readEntirePNGImage(const png_structp PNGPtr, const png_infop infoPTR);
	unsigned int _width;
	unsigned int _height;
	int _bitDepth;
	int _colorFormat;
	std::vector<uint8_t> _data;
};