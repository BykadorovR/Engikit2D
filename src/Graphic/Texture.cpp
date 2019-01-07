#include "texture.h"
#include "image.h"

Texture::Texture(string path) {
	RawImageData raw_image = load_png_asset_into_raw_image_data(&path[0]);
	data.assign(raw_image.data, raw_image.data + raw_image.size);
	width = raw_image.width;
	height = raw_image.height;
	delete raw_image.data;
}