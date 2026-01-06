#pragma once
#include "ModuleShaderDescriptors.h"

namespace tinygltf { class Model; struct Material; }

class BasicMaterial
{

	struct MaterialData
	{
		Vector4 baseColor;
		BOOL hasBaseColorTexture;
	};
public: 
	BasicMaterial();
	~BasicMaterial();

	void load(const tinygltf::Model& model, const tinygltf::Material& material, const char* filePath);

	ComPtr<ID3D12Resource> getMaterialBuffer() const { return materialBuffer; }
	ModuleShaderDescriptors* getShaderDescriptors() const { return shaderDescriptors; }

private:

	ComPtr<ID3D12Resource> colorTexture;
	ComPtr<ID3D12Resource> materialBuffer;
	UINT shaderDescriptorIndex = 0;
	MaterialData materialData;
	ModuleShaderDescriptors* shaderDescriptors = nullptr;

};

