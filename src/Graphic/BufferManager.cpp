#include "BufferManager.h"

BufferManager::BufferManager() {
	//VAO stores:
	//Calls to glEnableVertexAttribArray or glDisableVertexAttribArray.
	//Vertex attribute configurations via glVertexAttribPointer.
	//VBO associated with vertex attributes by calls to glVertexAttribPointer.
	//EBO 
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

bool BufferManager::changeBuffer(BufferType bufferType, std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall) {
	activateBuffer();
	getBuffer(bufferType)->change(position, sizeTarget, sizeOverall);
	deactivateBuffer();
	return false;
}

std::shared_ptr<Buffer> BufferManager::getBuffer(BufferType type) {
	for (auto buffer : _buffers) {
		if (buffer->getType() == type)
			return buffer;
	}
	return nullptr;
}

bool BufferManager::activateBuffer() {
	glBindVertexArray(_vao);
	return false;
}

bool BufferManager::deactivateBuffer() {
	glBindVertexArray(0);
	return false;
}