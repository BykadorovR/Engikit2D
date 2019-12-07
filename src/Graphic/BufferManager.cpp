#include "BufferManager.h"

BufferManager::BufferManager() {
	glGenVertexArrays(/*number of buffers to generate*/1, &_vao);
}

std::shared_ptr<Buffer> BufferManager::addBuffer(BufferType type, std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	activateBuffer();
	std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(type);
	buffer->create(position, sizeTarget, sizeOverall);
	_buffers.push_back(buffer);
	deactivateBuffer();
	return buffer;
}

bool BufferManager::changeBuffer(std::shared_ptr<Buffer> buffer, std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	activateBuffer();
	buffer->change(position, sizeTarget, sizeOverall);
	deactivateBuffer();
	return false;
}

bool BufferManager::activateBuffer() {
	glBindVertexArray(_vao);
	return false;
}

bool BufferManager::deactivateBuffer() {
	glBindVertexArray(0);
	return false;
}