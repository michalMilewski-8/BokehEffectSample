#include "Model.h"

Model::Model(std::string model_path, std::string texture_path,float scale,bool swap_yz )
{
	mesh = loadMesh(model_path.c_str(),scale,swap_yz);
	texture = loadTexture(texture_path.c_str());
}
