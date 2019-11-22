#pragma once
#include <vector>

#include <GL/glew.h>

enum BufferType {
	Position = 0,
	Texture = 1
};

class Buffer {
private:
	std::vector<float> _data;
	GLuint _vbo;
	static int _ID;
public:
	Buffer();
	bool create(BufferType type, std::tuple<float, float> position, std::tuple<float, float> size);
	bool change(int ID, std::tuple<float, float> position, std::tuple<float, float> size);
};