#pragma once
#include "platformGL.h"

#define BUFFER_OFFSET(i) ((void*)(i))

GLuint createVBO(const GLsizeiptr size, const GLvoid* data, const GLenum usage);