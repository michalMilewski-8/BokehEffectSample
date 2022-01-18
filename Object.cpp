#include "Object.h"
#include <glm/ext/matrix_transform.hpp>

Object::Object(std::shared_ptr<Model> model_,glm::vec3 pos_)
{
	model = model_;
	modelMatrix = glm::mat4(1);
	speed = 0;
	angle = 0;
	pos = pos_;
}

void Object::UpdateObject(float dt)
{
	//speed += dt / 2;
	//float step = dt * glm::sin(speed);
	//angle += step * 300;
	//while (angle > 360) angle -= 360;
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, pos);
	modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), { 0, 1, 0 });
}
