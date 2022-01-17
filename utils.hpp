#pragma once

#include <glad/glad.h>

GLuint loadShaderFromFile(const char* filename, GLenum shaderType);
GLuint loadShaders(const char* vs, const char* fs);
GLuint loadTexture(const char* filename);
