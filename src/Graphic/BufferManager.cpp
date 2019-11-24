#include "BufferManager.h"

BufferManager::BufferManager() {
	glGenVertexArrays(/*number of buffers to generate*/1, &_vao);
}

std::shared_ptr<Buffer> BufferManager::addBuffer(BufferType type, std::tuple<float, float> position, std::tuple<float, float> size) {
	glBindVertexArray(_vao);
	int bufferSize = sizeof(float) * /*rows*/ 4 * /*cols*/ 2;
	std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>();
	buffer->create(type, position, size);
	_buffers.push_back(buffer);
	return buffer;
}