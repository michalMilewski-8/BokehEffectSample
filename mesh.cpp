#include "mesh.hpp"

#include <array>
#include <iostream>
#include <fstream>
#include <iomanip>

Mesh loadMesh(const char *filename) {
	Mesh ret;

	std::vector<glm::vec3> loadedPositions;
	std::vector<glm::vec3> loadedNormals;
	std::vector<glm::vec2> loadedUvs;

	std::vector<unsigned int> pIndices, nIndices, uIndices;

	std::ifstream file(filename, std::ifstream::in);
	if (!file) {
		std::cout << "Couldn't open file: " << filename << '\n';
		exit(1);
	}

	while (true) {
		std::string header;

		file >> header;
		if (!file) break;

		if (header == "v") {
			glm::vec3 p;
			file >> p.x >> p.y >> p.z;
			loadedPositions.push_back(p);
		}

		if (header == "vt") {
			glm::vec2 t;
			file >> t.x >> t.y;
			loadedUvs.push_back(t);
		}

		if (header == "vn") {
			glm::vec3 n;
			file >> n.x >> n.y >> n.z;
			loadedNormals.push_back(n);
		}

		if (header == "f") {
			std::array<unsigned int, 3> ps, ns, us;
			file >> ps[0];
			file.ignore(1) >> us[0];
			file.ignore(1) >> ns[0];
			
			file >> ps[1];
			file.ignore(1) >> us[1];
			file.ignore(1) >> ns[1];
			
			file >> ps[2];
			file.ignore(1) >> us[2];
			file.ignore(1) >> ns[2];
			
			pIndices.push_back(ps[0]-1);
			nIndices.push_back(ns[0]-1);
			uIndices.push_back(us[0]-1);
			pIndices.push_back(ps[1]-1);
			nIndices.push_back(ns[1]-1);
			uIndices.push_back(us[1]-1);
			pIndices.push_back(ps[2]-1);
			nIndices.push_back(ns[2]-1);
			uIndices.push_back(us[2]-1);
		}
	}

	file.close();

	for (size_t i = 0; i < pIndices.size(); i++) {
		ret.positions.push_back(loadedPositions[pIndices[i]]);
		ret.normals.push_back(loadedNormals[nIndices[i]]);
		ret.uvs.push_back(loadedUvs[uIndices[i]]);
	}

	glGenVertexArrays(1, &ret.vao);
	glBindVertexArray(ret.vao);


	glGenBuffers(1, &ret.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.positions.size() * sizeof(glm::vec3), &ret.positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ret.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.normals.size() * sizeof(glm::vec3), &ret.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ret.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.uvs.size() * sizeof(glm::vec2), &ret.uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
	glEnableVertexAttribArray(2);

	ret.count = static_cast<unsigned int>(ret.positions.size());

	return ret;
}

Mesh makeQuad() {
	Mesh ret;

	ret.positions.push_back({-1, -1, 0});
	ret.normals.push_back({0, 0, 1});
	ret.uvs.push_back({0, 0});

	ret.positions.push_back({1, -1, 0});
	ret.normals.push_back({0, 0, 1});
	ret.uvs.push_back({1, 0});

	ret.positions.push_back({-1, 1, 0});
	ret.normals.push_back({0, 0, 1});
	ret.uvs.push_back({0, 1});

	ret.positions.push_back({-1, 1, 0});
	ret.normals.push_back({0, 0, 1});
	ret.uvs.push_back({0, 1});

	ret.positions.push_back({1, -1, 0});
	ret.normals.push_back({0, 0, 1});
	ret.uvs.push_back({1, 0});

	ret.positions.push_back({1, 1, 0});
	ret.normals.push_back({0, 0, 1});
	ret.uvs.push_back({1, 1});

	glGenVertexArrays(1, &ret.vao);
	glBindVertexArray(ret.vao);

	glGenBuffers(1, &ret.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.positions.size() * sizeof(glm::vec3), &ret.positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ret.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.normals.size() * sizeof(glm::vec3), &ret.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ret.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.uvs.size() * sizeof(glm::vec2), &ret.uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
	glEnableVertexAttribArray(2);

	ret.count = static_cast<unsigned int>(ret.positions.size());

	return ret;
}

Mesh makeCube() {
	Mesh ret;

	ret.positions.push_back({-1, -1, -1});
	ret.positions.push_back({1, -1, -1});
	ret.positions.push_back({-1, 1, -1});
	ret.positions.push_back({-1, 1, -1});
	ret.positions.push_back({1, -1, -1});
	ret.positions.push_back({1, 1, -1});
	ret.uvs.push_back({0./4., 2./3.});
	ret.uvs.push_back({1./4., 2./3.});
	ret.uvs.push_back({0./4., 1./3.});
	ret.uvs.push_back({0./4., 1./3.});
	ret.uvs.push_back({1./4., 2./3.});
	ret.uvs.push_back({1./4., 1./3.});

	ret.positions.push_back({1, -1, -1});
	ret.positions.push_back({1, -1, 1});
	ret.positions.push_back({1, 1, -1});
	ret.positions.push_back({1, 1, -1});
	ret.positions.push_back({1, -1, 1});
	ret.positions.push_back({1, 1, 1});
	ret.uvs.push_back({1./4., 2./3.});
	ret.uvs.push_back({2./4., 2./3.});
	ret.uvs.push_back({1./4., 1./3.});
	ret.uvs.push_back({1./4., 1./3.});
	ret.uvs.push_back({2./4., 2./3.});
	ret.uvs.push_back({2./4., 1./3.});

	ret.positions.push_back({-1, -1, 1});
	ret.positions.push_back({1, -1, 1});
	ret.positions.push_back({-1, 1, 1});
	ret.positions.push_back({-1, 1, 1});
	ret.positions.push_back({1, -1, 1});
	ret.positions.push_back({1, 1, 1});
	ret.uvs.push_back({3./4., 2./3.});
	ret.uvs.push_back({2./4., 2./3.});
	ret.uvs.push_back({3./4., 1./3.});
	ret.uvs.push_back({3./4., 1./3.});
	ret.uvs.push_back({2./4., 2./3.});
	ret.uvs.push_back({2./4., 1./3.});

	ret.positions.push_back({-1, -1, -1});
	ret.positions.push_back({-1, -1, 1});
	ret.positions.push_back({-1, 1, -1});
	ret.positions.push_back({-1, 1, -1});
	ret.positions.push_back({-1, -1, 1});
	ret.positions.push_back({-1, 1, 1});
	ret.uvs.push_back({4./4., 2./3.});
	ret.uvs.push_back({3./4., 2./3.});
	ret.uvs.push_back({4./4., 1./3.});
	ret.uvs.push_back({4./4., 1./3.});
	ret.uvs.push_back({3./4., 2./3.});
	ret.uvs.push_back({3./4., 1./3.});

	ret.positions.push_back({-1, 1, -1});
	ret.positions.push_back({-1, 1, 1});
	ret.positions.push_back({1, 1, -1});
	ret.positions.push_back({1, 1, -1});
	ret.positions.push_back({-1, 1, 1});
	ret.positions.push_back({1, 1, 1});
	ret.uvs.push_back({1./4., 0./3.});
	ret.uvs.push_back({2./4., 0./3.});
	ret.uvs.push_back({1./4., 1./3.});
	ret.uvs.push_back({1./4., 1./3.});
	ret.uvs.push_back({2./4., 0./3.});
	ret.uvs.push_back({2./4., 1./3.});

	ret.positions.push_back({-1, -1, -1});
	ret.positions.push_back({-1, -1, 1});
	ret.positions.push_back({1, -1, -1});
	ret.positions.push_back({1, -1, -1});
	ret.positions.push_back({-1, -1, 1});
	ret.positions.push_back({1, -1, 1});
	ret.uvs.push_back({1./4., 3./3.});
	ret.uvs.push_back({2./4., 3./3.});
	ret.uvs.push_back({1./4., 2./3.});
	ret.uvs.push_back({1./4., 2./3.});
	ret.uvs.push_back({2./4., 3./3.});
	ret.uvs.push_back({2./4., 2./3.});

	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});
	ret.normals.push_back({0, 0, 1});

	glGenVertexArrays(1, &ret.vao);
	glBindVertexArray(ret.vao);

	glGenBuffers(1, &ret.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.positions.size() * sizeof(glm::vec3), &ret.positions[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &ret.normalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.normalBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.normals.size() * sizeof(glm::vec3), &ret.normals[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ret.uvBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ret.uvBuffer);
	glBufferData(GL_ARRAY_BUFFER, ret.uvs.size() * sizeof(glm::vec2), &ret.uvs[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), 0);
	glEnableVertexAttribArray(2);

	ret.count = static_cast<unsigned int>(ret.positions.size());

	return ret;
}
