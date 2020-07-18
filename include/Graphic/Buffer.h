#pragma once
#include <vector>

#include <GL/glew.h>
#include <iostream>
#include <fstream>
#include <string>
#include <array>

enum BufferType {
	Position = 0,
	Texture = 1
};

//all operations with Buffer should go via BufferManager
class Buffer {
private:
	std::vector<float> _data;
	std::vector<int> _indexes;
	GLuint _vbo;
	GLuint _ebo;
	BufferType _type;
	std::ofstream myfile;
public:
	Buffer(BufferType type);
	bool create(std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall);
	bool change(std::tuple<float, float> position, std::tuple<float, float> sizeTarget, std::tuple<float, float> sizeOverall);
	GLuint getVBO();
	GLuint getEBO();
	BufferType getType();
};