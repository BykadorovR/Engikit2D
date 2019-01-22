#pragma once
#include "platformGL.h"

class Buffer {
public:
	int bindVBO(GLvoid* data, GLsizeiptr size, GLenum usage);
	GLuint getVBOObject();
private:
	GLuint vboObject;
};