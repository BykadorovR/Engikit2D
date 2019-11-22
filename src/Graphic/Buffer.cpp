#include <tuple>

#include "Buffer.h"
#include "State.h"

Buffer::Buffer() {
	glGenBuffers(1, &_vbo);
	_ID++;
}

bool Buffer::create(BufferType type, std::tuple<float, float> position, std::tuple<float, float> size) {
	float width = std::get<0>(size),
		  height = std::get<1>(size);
	float x = std::get<0>(position),
		  y = std::get<1>(position);
	float objectWidthN = (float)width / (float)std::get<0>(resolution);
	float objectHeightN = (float)height / (float)std::get<1>(resolution);
	float startX = (float)x / (float)std::get<0>(resolution);
	//by default for BufferType::Position
	float startY = (float)(std::get<1>(resolution) - y) / (float)std::get<1>(resolution);
	if (type == BufferType::Texture) {
		startY = (float)y / (float)std::get<1>(resolution);
		objectWidthN  *= -1; //In case of X,Y, Y increased from bot to top, in case of U,V, from top to bot
		objectHeightN *= -1; //so in data[] we will have only "+" sign everywhere
	}
	// Order of coordinates: X, Y
	// 0   2
	// | / |
	// 1   3
	float data[] = { startX,                startY,
					 startX,                startY - objectHeightN,
					 startX + objectWidthN, startY,
					 startX + objectWidthN, startY - objectHeightN };

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), nullptr, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}