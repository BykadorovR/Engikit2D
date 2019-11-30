#pragma once
#include <GL/glew.h>
#include <string>

class Shader {
public:
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	GLuint getProgram();
private:
	GLuint _program;

	GLuint compileShader(std::string source, GLenum type);
	GLuint linkProgram(const GLuint vertex_shader, const GLuint fragment_shader);
	GLuint buildProgram(
		std::string vertex_shader_source,
		std::string fragment_shader_source);

	GLuint buildProgramFromAsset(const char* vertex_shader_path, const char* fragment_shader_path);
	/* Should be called just before using a program to draw, if validation is needed. */
	GLint validateProgram(const GLuint program);
};