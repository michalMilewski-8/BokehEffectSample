#include "Model.h"

Model::Model(std::string model_path, std::string texture_path,float scale)
{
	mesh = loadMesh(model_path.c_str(),scale);
	texture = loadTexture(texture_path.c_str());
}
