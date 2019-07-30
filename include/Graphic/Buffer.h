#pragma once
#include "platformGL.h"

class Buffer {
public:
	int bindVBO(float* data, GLsizeiptr size, GLenum usage);
	GLuint getVBOObject();
	GLuint getVAOObject();
	float* getBuffer();
private:
	GLuint vboObject;
	GLuint vaoObject;
	float* _buffer;
};