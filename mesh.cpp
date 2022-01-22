#include "mesh.hpp"

#include <array>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <algorithm>

Mesh loadMesh(const char* filename, float scale) {
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
			t.y = 1.0f - t.y;
			loadedUvs.push_back(t);
		}

		if (header == "vn") {
			glm::vec3 n;
			file >> n.x >> n.y >> n.z;
			loadedNormals.push_back(n);
		}

		if (header == "f") {
			std::string line;
			std::stringstream ss;
			std::getline(file, line);
			int n = std::count(line.begin(), line.end(), '/');

			ss << line;
			if (n == 6)
			{
				std::array<unsigned int, 3> ps, ns, us;
				ss >> ps[0];
				ss.ignore(1) >> us[0];
				ss.ignore(1) >> ns[0];

				ss >> ps[1];
				ss.ignore(1) >> us[1];
				ss.ignore(1) >> ns[1];

				ss >> ps[2];
				ss.ignore(1) >> us[2];
				ss.ignore(1) >> ns[2];

				pIndices.push_back(ps[0] - 1);
				nIndices.push_back(ns[0] - 1);
				uIndices.push_back(us[0] - 1);
				pIndices.push_back(ps[1] - 1);
				nIndices.push_back(ns[1] - 1);
				uIndices.push_back(us[1] - 1);
				pIndices.push_back(ps[2] - 1);
				nIndices.push_back(ns[2] - 1);
				uIndices.push_back(us[2] - 1);
			}
			if (n == 8)
			{
				std::array<unsigned int, 4> ps, ns, us;
				ss >> ps[0];
				ss.ignore(1) >> us[0];
				ss.ignore(1) >> ns[0];

				ss >> ps[1];
				ss.ignore(1) >> us[1];
				ss.ignore(1) >> ns[1];

				ss >> ps[2];
				ss.ignore(1) >> us[2];
				ss.ignore(1) >> ns[2];

				ss >> ps[3];
				ss.ignore(1) >> us[3];
				ss.ignore(1) >> ns[3];

				pIndices.push_back(ps[0] - 1);
				nIndices.push_back(ns[0] - 1);
				uIndices.push_back(us[0] - 1);
				pIndices.push_back(ps[1] - 1);
				nIndices.push_back(ns[1] - 1);
				uIndices.push_back(us[1] - 1);
				pIndices.push_back(ps[2] - 1);
				nIndices.push_back(ns[2] - 1);
				uIndices.push_back(us[2] - 1);

				pIndices.push_back(ps[0] - 1);
				nIndices.push_back(ns[0] - 1);
				uIndices.push_back(us[0] - 1);
				pIndices.push_back(ps[2] - 1);
				nIndices.push_back(ns[2] - 1);
				uIndices.push_back(us[2] - 1);
				pIndices.push_back(ps[3] - 1);
				nIndices.push_back(ns[3] - 1);
				uIndices.push_back(us[3] - 1);
			}
		}
	}

	file.close();

	float min_x = loadedPositions[0].x;
	float min_y = loadedPositions[0].y;
	float min_z = loadedPositions[0].z;
	float max_x = loadedPositions[0].x;
	float max_y = loadedPositions[0].y;
	float max_z = loadedPositions[0].z;
	for (int i = 0; i < loadedPositions.size(); ++i)
	{
		auto& pos = loadedPositions[i];
		if (pos.x < min_x)min_x = pos.x;
		if (pos.y < min_y)min_y = pos.y;
		if (pos.z < min_z)min_z = pos.z;
		if (pos.x > max_x)max_x = pos.x;
		if (pos.y > max_y)max_y = pos.y;
		if (pos.z > max_z)max_z = pos.z;
	}

	glm::vec3 mid = { (min_x + max_x) / 2,
					(min_y + max_y) / 2,
					(min_z + max_z) / 2, };

	float scale_int_x = 2.0f / (max_x - min_x);
	float scale_int_y = 2.0f / (max_y - min_y);
	float scale_int_z = 2.0f / (max_z - min_z);
	float scale_int = std::min(std::min(scale_int_x, scale_int_x), scale_int_z);
	for (size_t i = 0; i < pIndices.size(); i++) {
		ret.positions.push_back(scale * scale_int * (loadedPositions[pIndices[i]] - mid));
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

	ret.positions.push_back({ -1, -1, 0 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.uvs.push_back({ 0, 0 });

	ret.positions.push_back({ 1, -1, 0 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.uvs.push_back({ 1, 0 });

	ret.positions.push_back({ -1, 1, 0 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.uvs.push_back({ 0, 1 });

	ret.positions.push_back({ -1, 1, 0 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.uvs.push_back({ 0, 1 });

	ret.positions.push_back({ 1, -1, 0 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.uvs.push_back({ 1, 0 });

	ret.positions.push_back({ 1, 1, 0 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.uvs.push_back({ 1, 1 });

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

Mesh makeCube(float size) {
	Mesh ret;

	ret.positions.push_back({ -size, -size, -size });
	ret.positions.push_back({ size, -size, -size });
	ret.positions.push_back({ -size, size, -size });
	ret.positions.push_back({ -size, size, -size });
	ret.positions.push_back({ size, -size, -size });
	ret.positions.push_back({ size, size, -size });
	ret.uvs.push_back({ 0. / 4., 2. / 3. });
	ret.uvs.push_back({ 1. / 4., 2. / 3. });
	ret.uvs.push_back({ 0. / 4., 1. / 3. });
	ret.uvs.push_back({ 0. / 4., 1. / 3. });
	ret.uvs.push_back({ 1. / 4., 2. / 3. });
	ret.uvs.push_back({ 1. / 4., 1. / 3. });

	ret.positions.push_back({ size, -size, -size });
	ret.positions.push_back({ size, -size, size });
	ret.positions.push_back({ size, size, -size });
	ret.positions.push_back({ size, size, -size });
	ret.positions.push_back({ size, -size, size });
	ret.positions.push_back({ size, size, size });
	ret.uvs.push_back({ 1. / 4., 2. / 3. });
	ret.uvs.push_back({ 2. / 4., 2. / 3. });
	ret.uvs.push_back({ 1. / 4., 1. / 3. });
	ret.uvs.push_back({ 1. / 4., 1. / 3. });
	ret.uvs.push_back({ 2. / 4., 2. / 3. });
	ret.uvs.push_back({ 2. / 4., 1. / 3. });

	ret.positions.push_back({ -size, -size, size });
	ret.positions.push_back({ size, -size, size });
	ret.positions.push_back({ -size, size, size });
	ret.positions.push_back({ -size, size, size });
	ret.positions.push_back({ size, -size, size });
	ret.positions.push_back({ size, size, size });
	ret.uvs.push_back({ 3. / 4., 2. / 3. });
	ret.uvs.push_back({ 2. / 4., 2. / 3. });
	ret.uvs.push_back({ 3. / 4., 1. / 3. });
	ret.uvs.push_back({ 3. / 4., 1. / 3. });
	ret.uvs.push_back({ 2. / 4., 2. / 3. });
	ret.uvs.push_back({ 2. / 4., 1. / 3. });

	ret.positions.push_back({ -size, -size, -size });
	ret.positions.push_back({ -size, -size, size });
	ret.positions.push_back({ -size, size, -size });
	ret.positions.push_back({ -size, size, -size });
	ret.positions.push_back({ -size, -size, size });
	ret.positions.push_back({ -size, size, size });
	ret.uvs.push_back({ 4. / 4., 2. / 3. });
	ret.uvs.push_back({ 3. / 4., 2. / 3. });
	ret.uvs.push_back({ 4. / 4., 1. / 3. });
	ret.uvs.push_back({ 4. / 4., 1. / 3. });
	ret.uvs.push_back({ 3. / 4., 2. / 3. });
	ret.uvs.push_back({ 3. / 4., 1. / 3. });

	ret.positions.push_back({ -size, size, -size });
	ret.positions.push_back({ -size, size, size });
	ret.positions.push_back({ size, size, -size });
	ret.positions.push_back({ size, size, -size });
	ret.positions.push_back({ -size, size, size });
	ret.positions.push_back({ size, size, size });
	ret.uvs.push_back({ 1. / 4., 0. / 3. });
	ret.uvs.push_back({ 2. / 4., 0. / 3. });
	ret.uvs.push_back({ 1. / 4., 1. / 3. });
	ret.uvs.push_back({ 1. / 4., 1. / 3. });
	ret.uvs.push_back({ 2. / 4., 0. / 3. });
	ret.uvs.push_back({ 2. / 4., 1. / 3. });

	ret.positions.push_back({ -size, -size, -size });
	ret.positions.push_back({ -size, -size, size });
	ret.positions.push_back({ size, -size, -size });
	ret.positions.push_back({ size, -size, -size });
	ret.positions.push_back({ -size, -size, size });
	ret.positions.push_back({ size, -size, size });
	ret.uvs.push_back({ 1. / 4., 3. / 3. });
	ret.uvs.push_back({ 2. / 4., 3. / 3. });
	ret.uvs.push_back({ 1. / 4., 2. / 3. });
	ret.uvs.push_back({ 1. / 4., 2. / 3. });
	ret.uvs.push_back({ 2. / 4., 3. / 3. });
	ret.uvs.push_back({ 2. / 4., 2. / 3. });

	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });
	ret.normals.push_back({ 0, 0, 1 });

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
