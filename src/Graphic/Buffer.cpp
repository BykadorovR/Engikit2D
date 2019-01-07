#include "Buffer.h"
#include <assert.h>
#include <stdlib.h>

GLuint createVBO(const GLsizeiptr size, const GLvoid* data, const GLenum usage) {
	assert(data != NULL);
	GLuint vboObject;
	glGenBuffers(1, &vboObject);
	assert(vboObject != 0);

	glBindBuffer(GL_ARRAY_BUFFER, vboObject);
	glBufferData(GL_ARRAY_BUFFER, size, data, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	return vboObject;
}