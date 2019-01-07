#pragma once
#include "platformGL.h"
#include "texture.h"

struct RawImageData {
	RawImageData(int _width, int _height, int _size, GLenum _gl_color_format, unsigned char* _data) : width(_width), height(_height), size(_size), 
	gl_color_format(_gl_color_format), data(_data) {

	}
	int width;
	int height;
	int size;
	//GL_RGBA 0x1908 see gl.h
	GLenum gl_color_format;
	unsigned char* data;
};

struct FileData {
	FileData(std::string _data = "") : data(_data) {

	}
	std::string data;
} ;

FileData get_asset_data(const char* relative_path);


/* Returns the decoded image data, or aborts if there's an error during decoding. */
RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size);
void join_image_to_result_image(Texture image, int start_x, int start_y, Texture& result_image);
void release_raw_image_data(const RawImageData* data);
GLuint load_texture(
	const GLsizei width, const GLsizei height,
	const GLenum type, const GLvoid* pixels);

RawImageData load_png_asset_into_raw_image_data(const char* relative_path);
GLuint load_raw_image_data_into_texture(Texture& raw_image_data);
GLuint build_program_from_assets(const char* vertex_shader_path, const char* fragment_shader_path);