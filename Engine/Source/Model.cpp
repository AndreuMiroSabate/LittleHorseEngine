#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION


#include "Globals.h"
#include "Model.h"
#include "tiny_gltf.h"

void Model::LoadModel(const char* fileName)
{
	tinygltf::TinyGLTF gltfContext;
	tinygltf::Model model;

	std::string error, warning;

	bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, fileName);

	if (loadOk)
	{
		//Implementation
	}
	else
	{
		LOG("Error loading %s: %s", fileName, error.c_str())
	}
}
