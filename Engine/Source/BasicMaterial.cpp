#include "Globals.h"
#include "BasicMaterial.h"

#include "Application.h"
#include "ModuleResources.h"

#include <string>

BasicMaterial::BasicMaterial()
{
}
BasicMaterial::~BasicMaterial()
{
}

void BasicMaterial::load(const tinygltf::Model& model, const tinygltf::Material& material, const char* filePath)
{
	Vector4 colour = Vector4(
		float(material.pbrMetallicRoughness.baseColorFactor[0]),
		float(material.pbrMetallicRoughness.baseColorFactor[1]),
		float(material.pbrMetallicRoughness.baseColorFactor[2]),
		float(material.pbrMetallicRoughness.baseColorFactor[3])
	);

	if(material.pbrMetallicRoughness.baseColorTexture.index >= 0)
	{
		const tinygltf::Texture& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
		const tinygltf::Image& image = model.images[texture.source];
		if(!image.uri.empty())
		{
			std::string fullPath = std::string(filePath) + image.uri;
			std::wstring wFullPath(fullPath.begin(), fullPath.end());
			colorTexture = app->getResources()->createTextureFromFile(wFullPath.c_str());
		}
	}
}
