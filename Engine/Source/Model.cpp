#include "Globals.h"
#include "Model.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#define TINYGLTF_IMPLEMENTATION

#include "tiny_gltf.h"

void Model::LoadModel(const char* fileName, const char* basePath, BasicMaterial::Type materialType)
{
	tinygltf::TinyGLTF gltfContext;
	tinygltf::Model model;

	std::string error, warning;

	bool loadOk = gltfContext.LoadASCIIFromFile(&model, &error, &warning, fileName);

	if (loadOk)
	{

		meshCount = static_cast<uint32_t>(model.meshes.size());
		meshes = std::make_unique<Mesh[]>(meshCount);
		int meshIndex = 0;

		materialCount = static_cast<uint32_t>(model.materials.size());
		materials = std::make_unique<BasicMaterial[]>(materialCount);
		int materialIndex = 0;

		for (const auto& sMeshes : model.meshes)
		{
			for (const auto& primitive : sMeshes.primitives)
			{
				meshes[meshIndex++].loadMesh(model, sMeshes, primitive);
			}
		}
		for (const auto& sMaterial : model.materials)
		{
			materials[materialIndex++].load(model, sMaterial, materialType, basePath);
		}
	}
	else
	{
		LOG("Error loading %s: %s", fileName, error.c_str())
	}
}
