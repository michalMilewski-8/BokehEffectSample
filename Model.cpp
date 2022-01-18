#include "Model.h"

Model::Model(std::string model_path, std::string texture_path)
{
	mesh = loadMesh(model_path.c_str());
	texture = loadTexture(texture_path.c_str());
}
