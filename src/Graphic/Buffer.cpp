#include <tuple>

#include "Buffer.h"

Buffer::Buffer(BufferType type) {
	glGenBuffers(1, &_vbo);
	_type = type;
}

/*
Let's assume for now that 1 object = 1 buffer, so no impl for N objects = 1 buffer;
*/
bool Buffer::create(std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	float width = std::get<0>(sizeTarget),
		  height = std::get<1>(sizeTarget);
	float x = std::get<0>(position),
		  y = std::get<1>(position);
	float objectWidthN = (float)width / (float)std::get<0>(sizeOverall);
	float objectHeightN = (float)height / (float)std::get<1>(sizeOverall);
	float startX = (float)x / (float)std::get<0>(sizeOverall);
	//by default for BufferType::Position
	float startY = (float)(std::get<1>(sizeOverall) - y) / (float)std::get<1>(sizeOverall);
	if (_type == BufferType::Texture) {
		startY = (float)y / (float)std::get<1>(sizeOverall);
		objectHeightN *= -1; //so in data[] we will have only "+" sign everywhere
	}
	// Order of coordinates: X, Y
	// 0   2
	// | / |
	// 1   3
	_data = { startX,                startY,
			  startX,                startY - objectHeightN,
			  startX + objectWidthN, startY,
			  startX + objectWidthN, startY - objectHeightN };

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_data), &_data[0], GL_STATIC_DRAW);
	if (_type == BufferType::Position) {
		glEnableVertexAttribArray(/*location = 0*/0);
		glVertexAttribPointer(/*location = 0*/0, /*position count*/2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	else {
		glEnableVertexAttribArray(/*location = 1*/1);
		glVertexAttribPointer(/*location = 1*/1, /*position count*/2, GL_FLOAT, GL_FALSE, 0, 0);
	}
	return false;
}

BufferType Buffer::getType() {
	return _type;
}

bool Buffer::change(std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	float width = std::get<0>(sizeTarget),
		height = std::get<1>(sizeTarget);
	float x = std::get<0>(position),
		y = std::get<1>(position);
	float objectWidthN = (float)width / (float)std::get<0>(sizeOverall);
	float objectHeightN = (float)height / (float)std::get<1>(sizeOverall);
	float startX = (float)x / (float)std::get<0>(sizeOverall);
	//by default for BufferType::Position
	float startY = (float)(std::get<1>(sizeOverall) - y) / (float)std::get<1>(sizeOverall);
	if (_type == BufferType::Texture) {
		startY = (float)y / (float)std::get<1>(sizeOverall);
		objectHeightN *= -1; //so in data[] we will have only "+" sign everywhere
	}
	// Order of coordinates: X, Y
	// 0   2
	// | / |
	// 1   3
	_data = { startX,                startY,
			  startX,                startY - objectHeightN,
			  startX + objectWidthN, startY,
			  startX + objectWidthN, startY - objectHeightN };

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(_data), &_data[0]);
	if (_type == BufferType::Position) {
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