#pragma once
#include <vector>

#include <GL/glew.h>

enum BufferType {
	Position = 0,
	Texture = 1
};

//all operations with Buffer should go via BufferManager
class Buffer {
private:
	std::vector<float> _data;
	GLuint _vbo;
public:
	Buffer();
	bool create(BufferType type, std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall);
	bool change(std::tuple<float, float> position, std::tuple<float, float> size);
	GLuint getVBO();
};