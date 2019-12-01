#include <tuple>

#include "Buffer.h"
#include "State.h"

Buffer::Buffer() {
	glGenBuffers(1, &_vbo);
}

/*
Let's assume for now that 1 object = 1 buffer, so no impl for N objects = 1 buffer;
*/
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
	if (type == BufferType::Position) {
		glEnableVertexAttribArray(/*location = 0*/0);
		glVertexAttribPointer(/*location = 0*/0, /*position count*/2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else {
		glEnableVertexAttribArray(/*location = 1*/1);
		glVertexAttribPointer(/*location = 1*/1, /*position count*/2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	return false;
}

GLuint Buffer::getVBO() {
	return _vbo;
}