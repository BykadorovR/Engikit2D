#pragma once
#include "PlatformGL.h"

GLuint compileShader(const GLenum type, const GLchar* source, const GLint length);
GLuint linkProgram(const GLuint vertex_shader, const GLuint fragment_shader);
GLuint buildProgram(
	const GLchar * vertex_shader_source, const GLint vertex_shader_source_length,
	const GLchar * fragment_shader_source, const GLint fragment_shader_source_length);

/* Should be called just before using a program to draw, if validation is needed. */
GLint validateProgram(const GLuint program);