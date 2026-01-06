#pragma once
#include "Mesh.h"
#include "BasicMaterial.h"
class Model
{

public:

	std::vector<Mesh*> meshes;
	//std::vector<Nodes> nodes;
	//std::vector<Animation> animations;
	std::vector<BasicMaterial*> materials;

	void LoadModel(const char* fileName);
};

