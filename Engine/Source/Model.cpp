#include "Globals.h"
#include "Model.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION

#include "tiny_gltf.h"

void Model::LoadModel(const char* fileName)
{
	tinygltf::TinyGLTF gltfContext;
	tinygltf::Model model;

	std::string error, warning;

	bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, fileName);

	if (loadOk)
	{
		for (const auto& sMeshes : model.meshes)
		{
			for (const auto& primitive : sMeshes.primitives)
			{
				Mesh* mesh = new Mesh;
				mesh->loadMesh(model, sMeshes, primitive);
				meshes.push_back(mesh);
			}
		}
		for (const auto& sMaterial : model.materials)
		{
			BasicMaterial* material = new BasicMaterial;
			material->load(model, sMaterial, fileName);
			materials.push_back(material);
		}
	}
	else
	{
		LOG("Error loading %s: %s", fileName, error.c_str())
	}
}
