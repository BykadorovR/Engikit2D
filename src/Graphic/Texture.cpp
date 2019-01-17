#include "Texture.h"
#include "Image.h"
#include <assert.h>

Texture::Texture(string path) {
	ImageLoader loader;
	loader.loadPNG(&path[0]);
	width = loader.getWidth();
	height = loader.getHeight();
	data = loader.getData();
}

Texture::Texture(int _width, int _height) {
	width = _width;
	height = _height;
	data = std::vector<unsigned char>(width*height*4);
}

void Texture::addTexture(Texture image, int start_x, int start_y) {
	//here we hardcode RGBA format using 4 byte to represent 1 color
	int depth_color = 4;
	for (int y = 0; y < image.getHeight(); y++)
		for (int x = 0; x < image.getWidth() * depth_color; x++) {
			int coord_x_into_result = start_x * depth_color + x;
			int coord_y_into_result = start_y + y;
			assert(coord_x_into_result < width * depth_color);
			assert(coord_y_into_result < height);
			int coord_into_result = coord_x_into_result + width * depth_color * coord_y_into_result;
			int coord_into_source = x + image.getWidth() * depth_color * y;
			assert(coord_into_result < width * height * depth_color);
			assert(coord_into_source < image.getHeight() * image.getWidth() * depth_color);
			data[coord_into_result] = image[coord_into_source];
		}
}

GLuint Texture::loadTexture() {
	GLuint texture_object_id;
	GLenum argb_format = 0x1908;
	glGenTextures(1, &texture_object_id);
	assert(texture_object_id != 0);

	glBindTexture(GL_TEXTURE_2D, texture_object_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, argb_format, width, height, 0, argb_format, GL_UNSIGNED_BYTE, &data[0]);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture_object_id;
}