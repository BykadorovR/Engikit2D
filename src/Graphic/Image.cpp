#include "Image.h"
#include "Shader.h"

#include <assert.h>
#include <png.h>
#include <string>
#include <fstream>
#include <iostream>

struct DataHandle {
	DataHandle(png_byte* _data, png_size_t _size) : data(_data), size(_size) {

	}
	png_byte* data;
	png_size_t size;
};

struct ReadDataHandle {
	ReadDataHandle(DataHandle _data, png_size_t _offset) : data(_data), offset(_offset) {

	}
	DataHandle data;
	png_size_t offset;
};

struct PngInfo {
	PngInfo(png_uint_32 _width, png_uint_32 _height, int _color_type) : width(_width), height(_height), color_type(_color_type) {

	}
	const png_uint_32 width;
	const png_uint_32 height;
	const int color_type;
};


void join_image_to_result_image(Texture image, int start_x, int start_y, Texture& result_image) {
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

GLuint load_texture(
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

static void read_png_data_callback(
	png_structp png_ptr, png_byte* png_data, png_size_t read_length);
static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr);
static DataHandle read_entire_png_image(
	const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height);
static GLenum get_gl_color_format(const int png_color_format);

RawImageData get_raw_image_data_from_png(const void* png_data, const int png_data_size) {
	assert(png_data != NULL && png_data_size > 8);

	assert(png_check_sig((png_const_bytep) png_data, 8));

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	assert(png_ptr != NULL);
	png_infop info_ptr = png_create_info_struct(png_ptr);
	assert(info_ptr != NULL);

	ReadDataHandle png_data_handle(DataHandle((png_byte*) png_data, (png_size_t) png_data_size), 0 );
	png_set_read_fn(png_ptr, &png_data_handle, read_png_data_callback);

	if (setjmp(png_jmpbuf(png_ptr))) {
		assert("Error reading PNG file!");
	}

	const PngInfo png_info = read_and_update_info(png_ptr, info_ptr);
	const DataHandle raw_image = read_entire_png_image(png_ptr, info_ptr, png_info.height);

	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	return RawImageData(
		(int)png_info.width,
			(int)png_info.height,
			(int)raw_image.size,
			get_gl_color_format(png_info.color_type),
			(unsigned char*)raw_image.data);
}

static void read_png_data_callback(png_structp png_ptr, png_byte* raw_data, png_size_t read_length) {
	ReadDataHandle* handle = (ReadDataHandle*)png_get_io_ptr(png_ptr);
	const png_byte* png_src = handle->data.data + handle->offset;

	memcpy(raw_data, png_src, read_length);
	handle->offset += read_length;
}

static PngInfo read_and_update_info(const png_structp png_ptr, const png_infop info_ptr) {
	png_uint_32 width, height;
	int bit_depth, color_type;

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);

	// Convert transparency to full alpha
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png_ptr);

	// Convert grayscale, if needed.
	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
		png_set_expand_gray_1_2_4_to_8(png_ptr);

	// Convert paletted images, if needed.
	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_palette_to_rgb(png_ptr);

	// Add alpha channel, if there is none (rationale: GL_RGBA is faster than GL_RGB on many GPUs)
	if (color_type == PNG_COLOR_TYPE_PALETTE || color_type == PNG_COLOR_TYPE_RGB)
		png_set_add_alpha(png_ptr, 0xFF, PNG_FILLER_AFTER);

	// Ensure 8-bit packing
	if (bit_depth < 8)
		png_set_packing(png_ptr);
	else if (bit_depth == 16)
		png_set_scale_16(png_ptr);

	png_read_update_info(png_ptr, info_ptr);

	// Read the new color type after updates have been made.
	color_type = png_get_color_type(png_ptr, info_ptr);

	return PngInfo(width, height, color_type);
}

static DataHandle read_entire_png_image(const png_structp png_ptr, const png_infop info_ptr, const png_uint_32 height) {
	const png_size_t row_size = png_get_rowbytes(png_ptr, info_ptr);
	const int data_length = row_size * height;
	assert(row_size > 0);

	png_byte* raw_image = new png_byte[data_length];
	assert(raw_image != NULL);

	png_byte** row_ptrs = new png_byte*[height];

	png_uint_32 i;
	for (i = 0; i < height; i++) {
		row_ptrs[i] = &raw_image[i * row_size];
	}

	png_read_image(png_ptr, row_ptrs);

	return DataHandle(raw_image, (png_size_t)data_length);
}

static GLenum get_gl_color_format(const int png_color_format) {
	assert(png_color_format == PNG_COLOR_TYPE_GRAY
		|| png_color_format == PNG_COLOR_TYPE_RGB_ALPHA
		|| png_color_format == PNG_COLOR_TYPE_GRAY_ALPHA);

	switch (png_color_format) {
	case PNG_COLOR_TYPE_GRAY:
		return GL_LUMINANCE;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		return GL_RGBA;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		return GL_LUMINANCE_ALPHA;
	}

	return 0;
}

RawImageData load_png_asset_into_raw_image_data(const char* relative_path) {
	assert(relative_path != NULL);
	const FileData png_file = get_asset_data(relative_path);
	const RawImageData raw_image_data = get_raw_image_data_from_png(png_file.data.c_str(),
		png_file.data.size());

	return raw_image_data;
}

FileData get_asset_data(const char* relative_path) {
	std::string content;
	std::ifstream fileStream(relative_path, std::ios::binary);

	if (!fileStream.is_open()) {
		std::cerr << "Could not read file " << relative_path << ". File does not exist." << std::endl;
		return FileData();
	}

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		content.append(line + "\n");
	}
	fileStream.close();
	return (FileData) content;
}

GLuint load_raw_image_data_into_texture(Texture& raw_image_data) {
	//If we want join two images we need to create new class with RGBA channels because raw_image_data.data - unsigned char
	GLenum argb_format = 0x1908;
	const GLuint texture_object_id = load_texture(
		raw_image_data.getWidth(), raw_image_data.getHeight(), argb_format, raw_image_data.getData());
	return texture_object_id;
}

GLuint build_program_from_assets(const char* vertex_shader_path, const char* fragment_shader_path) {
	assert(vertex_shader_path != NULL);
	assert(fragment_shader_path != NULL);

	const FileData vertex_shader_source = get_asset_data(vertex_shader_path);
	const FileData fragment_shader_source = get_asset_data(fragment_shader_path);
	const GLuint program_object_id = buildProgram(
		(const GLchar*)vertex_shader_source.data.c_str(), vertex_shader_source.data.size(),
		(const GLchar*)fragment_shader_source.data.c_str(), fragment_shader_source.data.size());

	return program_object_id;
}
