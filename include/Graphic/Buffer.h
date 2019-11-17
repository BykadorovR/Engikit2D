#pragma once
#include <vector>

enum BufferType {
	Position = 0,
	Texture = 1
};

class Buffer {
private:
	std::vector<float> _data;
	GLuint _vbo;
	int _ID;
public:
	Buffer(int size);
	bool add(BufferType type, std::tuple<int, int> position, std::tuple<int, int> size);
	bool change(int ID, std::tuple<int, int> position, std::tuple<int, int> size);
};