#include "Shader.h"
#include <Windows.h>
#include <sstream>
#include <iostream>
#include <vector>
#include "Common.h"
#include <cassert>

GLuint Shader::compileShader(std::string source, GLenum type) {
	GLuint shader_object_id = glCreateShader(type);
	GLint compile_status = GL_FALSE;

	assert(shader_object_id != 0);

	int size = source.size();
	auto shaderText = source.c_str();
	glShaderSource(shader_object_id, 1, &shaderText, &size);
	glCompileShader(shader_object_id);
	glGetShaderiv(shader_object_id, GL_COMPILE_STATUS, &compile_status);
	if (compile_status == 0) {
		GLint maxLength = 0;
		glGetShaderiv(shader_object_id, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(shader_object_id, maxLength, &maxLength, &errorLog[0]);
		std::string textLog(&errorLog[0]);
		std::ostringstream os;
		os << textLog;
		OutputDebugString(os.str().c_str());

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(shader_object_id); // Don't leak the shader.
	}
	assert(compile_status != 0);


	return shader_object_id;
}

GLuint Shader::linkProgram(const GLuint vertex_shader, const GLuint fragment_shader) {
	GLuint program_object_id = glCreateProgram();
	GLint link_status;

	assert(program_object_id != 0);

	glAttachShader(program_object_id, vertex_shader);
	glAttachShader(program_object_id, fragment_shader);
	glLinkProgram(program_object_id);
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);
	glGetProgramiv(program_object_id, GL_LINK_STATUS, &link_status);
	assert(link_status != 0);
	return program_object_id;
}

GLuint Shader::buildProgram(
	std::string vertex_shader_source,
	std::string fragment_shader_source) {
	GLuint vertex_shader = compileShader(
		vertex_shader_source, GL_VERTEX_SHADER);
	GLuint fragment_shader = compileShader(
		fragment_shader_source, GL_FRAGMENT_SHADER);
	return linkProgram(vertex_shader, fragment_shader);
}

GLuint Shader::buildProgramFromAsset(const char* vertex_shader_path, const char* fragment_shader_path) {
	assert(vertex_shader_path != NULL);
	assert(fragment_shader_path != NULL);

	std::string vertex_shader_source = rawFileContent(vertex_shader_path);
	std::string fragment_shader_source = rawFileContent(fragment_shader_path);
	const GLuint program_object_id = buildProgram(vertex_shader_source, fragment_shader_source);

	return program_object_id;
}

GLint Shader::validateProgram(const GLuint program)
{
	return GLint();
}

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
	_program = buildProgramFromAsset(vertexShaderPath, fragmentShaderPath);
}

GLuint Shader::getProgram() {
	return _program;
}