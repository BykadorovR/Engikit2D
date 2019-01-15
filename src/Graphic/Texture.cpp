#include "Texture.h"
#include "Image.h"
#include <assert.h>

Texture::Texture(string path) {
	loader.loadPNG(&path[0]);
}

GLuint Texture::loadRawImageToTexture(Texture& rawImageData) {
	//If we want join two images we need to create new class with RGBA channels because raw_image_data.data - unsigned char
	GLenum argb_format = 0x1908;
	const GLuint texture_object_id = loadTexture(
		rawImageData.getWidth(), rawImageData.getHeight(), argb_format, rawImageData.getData());
	return texture_object_id;
}

void Texture::joinTextures(Texture image, int start_x, int start_y, Texture& result_image) {
	//here we hardcode RGBA format using 4 byte to represent 1 color
	int depth_color = 4;
	for (int y = 0; y < image.getHeight(); y++)
		for (int x = 0; x < image.getWidth() * depth_color; x++) {
			int coord_x_into_result = start_x * depth_color + x;
			int coord_y_into_result = start_y + y;
			assert(coord_x_into_result < result_image.getWidth() * depth_color);
			assert(coord_y_into_result < result_image.getHeight());
			int coord_into_result = coord_x_into_result + result_image.getWidth() * depth_color * coord_y_into_result;
			int coord_into_source = x + image.getWidth() * depth_color * y;
			assert(coord_into_result < result_image.getWidth() * result_image.getHeight() * depth_color);
			assert(coord_into_source < image.getHeight() * image.getWidth() * depth_color);
			result_image[coord_into_result] = image[coord_into_source];
		}
}

GLuint Texture::loadTexture(
	const GLsizei width, const GLsizei height,
	const GLenum type, const GLvoid* pixels) {
	GLuint texture_object_id;
	glGenTextures(1, &texture_object_id);
	assert(texture_object_id != 0);

	glBindTexture(GL_TEXTURE_2D, texture_object_id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, type, width, height, 0, type, GL_UNSIGNED_BYTE, pixels);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texture_object_id;
}