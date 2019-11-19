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
	bool addBuffer(BufferType type, std::tuple<int, int> position, std::tuple<int, int> size);
	bool changeBuffer(int ID, std::tuple<int, int> position, std::tuple<int, int> tuple);
};