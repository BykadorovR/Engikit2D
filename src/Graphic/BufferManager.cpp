#include "BufferManager.h"

BufferManager::BufferManager() {
	glGenVertexArrays(/*number of buffers to generate*/1, &_vao);
}

std::shared_ptr<Buffer> BufferManager::addBuffer(BufferType type, std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	activateBuffer();
	std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>();
	buffer->create(type, position, sizeTarget, sizeOverall);
	_buffers.push_back(buffer);
	deactivateBuffer();
	return buffer;
}

bool BufferManager::activateBuffer() {
	glBindVertexArray(_vao);
	return false;
}

bool BufferManager::deactivateBuffer() {
	glBindVertexArray(0);
	return false;
}