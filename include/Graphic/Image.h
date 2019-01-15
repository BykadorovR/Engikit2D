#pragma once
#include "platformGL.h"
#include "texture.h"
#include <png.h>

//needed for internal libPNG conversions
struct DataHandle {
	DataHandle(png_byte* _data, png_size_t _size, png_size_t _offset) : data(_data), size(_size), offset(_offset) {

	}
	png_byte* data;
	png_size_t size;
	png_size_t offset;
};

//needed for internal libPNG function
void PNGCallback(png_structp PNGPtr, png_byte* rawData, png_size_t readLength) {
	DataHandle* handle = (DataHandle*)png_get_io_ptr(PNGPtr);
	const png_byte* PNGSrc = handle->data + handle->offset;

	memcpy(rawData, PNGSrc, readLength);
	handle->offset += readLength;
}

class ImageLoader {
public:
	//return status
	int loadPNG(const char* relativePath);
	GLenum getGLColorFormat();
	unsigned int getWidth();
	unsigned int getHeight();
	int getBitdepth();
	vector<unsigned char> getData();

private:
	int readAndUpdateInfo(png_structp PNGPtr, png_infop infoPTR);
	int getImageFromPNG(std::string PNGData);
	int readEntirePNGImage(const png_structp PNGPtr, const png_infop infoPTR);
	unsigned int width;
	unsigned int height;
	int bitDepth;
	int colorFormat;
	vector<unsigned char> data;
};