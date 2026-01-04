#pragma once

#include "tiny_gltf.h"
class BasicMaterial
{
public: 
	BasicMaterial();
	~BasicMaterial();

	void load(const tinygltf::Model& model, const tinygltf::Material& material, const char* filePath);

private:

	ComPtr<ID3D12Resource> colorTexture;

};

