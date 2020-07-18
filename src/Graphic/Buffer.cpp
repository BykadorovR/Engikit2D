#include <tuple>

#include "Buffer.h"

Buffer::Buffer(BufferType type) {
	//create 1 buffer with ID = _vbo
	glGenBuffers(1, &_vbo);
	//create EBO buffer
	glGenBuffers(1, &_ebo);
	_type = type;
}

/*
Let's assume for now that 1 object = 1 buffer, so no impl for N objects = 1 buffer;
*/
bool Buffer::create(std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	//using int because coords and size should be in "pixels" which are int
	//if use float, some visual glitches can appear
	int width = std::get<0>(sizeTarget),
		height = std::get<1>(sizeTarget);
	int x = std::get<0>(position),
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

	_indexes = { 0, 1, 2,
				 1, 2, 3 };

	//bind our buffer to OpenGL buffer from OpenGL internal state (so like assign pointer to our buffer to OpenGL buffer in internal state), we can work only with binded buffers
	//From that point on any buffer calls we make (on the GL_ARRAY_BUFFER target) will be used to configure the currently bounded buffer, which is _vbo
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	//copies previously defined vertex data into the OpenGL buffer's memory
	//GL_DYNAMIC_DRAW: the data is changed a lot and used many times
	//IMPORTANT: we should copy data EACH time if no VAO, because VAO stores state for us
	glBufferData(GL_ARRAY_BUFFER, _data.size() * sizeof(float), &_data[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexes.size() * sizeof(int), &_indexes[0], GL_DYNAMIC_DRAW);
	if (_type == BufferType::Position) {
		glEnableVertexAttribArray(/*location = 0*/0);
		//Each vertex attribute takes its data from memory managed by a VBO and which VBO it takes its data from(you can have multiple VBOs) is determined by the VBO currently bound to GL_ARRAY_BUFFER when calling glVertexAttribPointer
		//we also can set stride as 0 because our data is tightly packed so OpenGL calculate stride by itself, but it's always better to calculate it manually
		glVertexAttribPointer(/*location = 0*/0, /*position count*/2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	}
	else {
		glEnableVertexAttribArray(/*location = 1*/1);
		glVertexAttribPointer(/*location = 1*/1, /*position count*/2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	}
	return false;
}

BufferType Buffer::getType() {
	return _type;
}

bool Buffer::change(std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	//using int because coords and size should be in "pixels" which are int
	//if use float, some visual glitches can appear
	int width = std::get<0>(sizeTarget),
		height = std::get<1>(sizeTarget);
	int x = std::get<0>(position),
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
	glBufferSubData(GL_ARRAY_BUFFER, 0, _data.size() * sizeof(float), &_data[0]);
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

GLuint Buffer::getEBO() {
	return _vbo;
}