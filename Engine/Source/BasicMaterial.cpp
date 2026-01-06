#include "Globals.h"
#include "BasicMaterial.h"

#include "Application.h"
#include "ModuleResources.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE 

#include "tiny_gltf.h"

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

		const tinygltf::Texture& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
		const tinygltf::Image& image = model.images[texture.source];

		if(!image.uri.empty())
		{
			std::string fullPath = std::string(filePath) + image.uri;
			std::wstring wFullPath(fullPath.begin(), fullPath.end());
			colorTexture = app->getResources()->createTextureFromFile(wFullPath.c_str(), true);

			materialData.baseColor = colour;
			materialData.hasBaseColorTexture = TRUE;


			shaderDescriptors = app->getShaderDescriptors();
			shaderDescriptorIndex = shaderDescriptors->allocteDescriptor();
			app->getShaderDescriptors()->createSRV(colorTexture.Get(), 0);		
			
		}
		else
		{
			app->getShaderDescriptors()->createNullTexture2DSRV();
			materialData.baseColor = colour;
			materialData.hasBaseColorTexture = FALSE;
		}

	materialBuffer = app->getResources()->CreateDefaultBuffer(&materialData, sizeof(MaterialData), "Material Buffer");
}
