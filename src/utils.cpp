#include "utils.hpp"

#include <iostream>
#include <fstream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

GLuint loadShaderFromFile(const char* filename, GLenum shaderType) {

	std::ifstream file;
	file.open(filename, std::ifstream::in | std::ifstream::binary);

	file.seekg(0, std::ios::end);
	auto length = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(length);
	file.read(buffer.data(), length);
	file.close();

	GLuint shader = glCreateShader(shaderType);
	
	GLchar* fileContent = buffer.data();
	GLint fileLength = static_cast<GLint>(length);

	glShaderSource(shader, 1, &fileContent, &fileLength);
	glCompileShader(shader);

	int success;
	char log[1024];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(shader, 1024, nullptr, log);
		std::cout << filename << ":" << '\n' << log << '\n';
		exit(1);
	}

	return shader;
}

GLuint loadShaders(const char* vs, const char *fs) {
	GLuint program = glCreateProgram();
	int success;
	char log[1024];

	GLuint vertexShader = loadShaderFromFile(vs, GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShaderFromFile(fs, GL_FRAGMENT_SHADER);

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, 1024, nullptr, log);
		std::cout << log << '\n';
		exit(1);
	}

	return program;
}

GLuint loadTexture(const char *filename) {
	GLuint ret;
	glGenTextures(1, &ret);
	glBindTexture(GL_TEXTURE_2D, ret);

	int texWidth, texHeight, texChannels;
	float *texData = stbi_loadf(filename, &texWidth, &texHeight, &texChannels, 3);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, texWidth, texHeight, 0, GL_RGB, GL_FLOAT, texData);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	float anisotropy = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);
	stbi_image_free(texData);

	return ret;
}