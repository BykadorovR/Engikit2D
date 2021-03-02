#pragma once
#include <GL/glew.h>
#include <string>
#include <map>
#include <memory>

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

class ShaderStore {
private:
	std::map<std::string, std::shared_ptr<Shader> > _shaders;
public:
	static ShaderStore* instance() {
		static ShaderStore shaderStore;
		return &shaderStore;
	}

	bool addShader(std::string name, std::shared_ptr<Shader> shader) {
		_shaders.insert({ name, shader });
		return false;
	}

	std::shared_ptr<Shader> getShader(std::string name) {
		return _shaders[name];
	}
};
