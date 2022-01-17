#pragma once

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <vector>

struct Mesh {
	GLuint vao;
	GLuint vertexBuffer;
	GLuint normalBuffer;
	GLuint uvBuffer;
	unsigned int count;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
};

Mesh loadMesh(const char* filename);
Mesh makeQuad();
Mesh makeCube();
