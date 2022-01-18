#pragma once
#include "Model.h"
#include <memory>
struct Object
{
	Object(std::shared_ptr<Model> model_,glm::vec3 pos_);
	std::shared_ptr<Model> model;
	glm::mat4 modelMatrix;
	float speed;
	float angle;
	glm::vec3 pos;
	void UpdateObject(float dt);
};