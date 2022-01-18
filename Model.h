#pragma once
#include "mesh.hpp"
#include "utils.hpp"
#include <string>
class Model
{
public:
	Model(std::string model_path, std::string texture_path,float scale);
	GLuint texture;
	Mesh mesh;
};