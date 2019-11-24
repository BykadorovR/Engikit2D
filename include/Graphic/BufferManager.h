#pragma once
#include <tuple>
#include <memory>
#include <GL/glew.h>

#include "Buffer.h"

class BufferManager {
private:
	GLuint _vao;
	std::vector<std::shared_ptr<Buffer> > _buffers;
public:
	//need to create VAO
	BufferManager();
	std::shared_ptr<Buffer> addBuffer(BufferType type, std::tuple<float, float> position, std::tuple<float, float> size);
	bool changeBuffer(std::shared_ptr<Buffer> buffer, std::tuple<float, float> position, std::tuple<float, float> tuple);
};