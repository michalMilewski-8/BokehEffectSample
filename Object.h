#pragma once
#include "mesh.hpp"
struct Object
{
	Object(Mesh mesh_,float scale_);
	Mesh mesh;
	glm::mat4 modelMatrix;
	glm::mat4 oldModelMatrix;
	float speed;
	float angle;
	float scale;
	void UpdateObject(float dt);
};