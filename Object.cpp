#include "Object.h"
#include <glm/ext/matrix_transform.hpp>

Object::Object(Mesh mesh_,float scale_)
{
	mesh = mesh_;
	modelMatrix = glm::mat4(1);
	oldModelMatrix = glm::mat4(1);
	speed = 0;
	angle = 0;
	scale = scale_;
}

void Object::UpdateObject(float dt)
{
	oldModelMatrix = modelMatrix;
	speed += dt / 2;
	float step = dt * glm::sin(speed);
	angle += step * 300;
	while (angle > 360) angle -= 360;
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, { 0, glm::sin(glm::radians(angle)), 3 });
	modelMatrix = glm::rotate(modelMatrix, glm::radians(angle), { 0, 1, 0 });
	modelMatrix = glm::scale(modelMatrix, { scale,scale,scale });
}
