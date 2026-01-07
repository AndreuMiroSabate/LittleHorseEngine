#pragma once
#include "ModuleShaderDescriptors.h"

namespace tinygltf { class Model; struct Material; }

struct MaterialData
{
	Vector4 baseColor;
	BOOL hasBaseColorTexture;
};

struct PhongMaterialData
{
	XMFLOAT4 diffuseColour;
	float kDifusse;
	float kSpecular;
	float kShininess;
	BOOL hasDiffuseTex;
};

class BasicMaterial
{

public: 

	enum Type
	{
		BASIC = 0,
		PHONG
	};

public:

	BasicMaterial();
	~BasicMaterial();

	void load(const tinygltf::Model& model, const tinygltf::Material& material, Type materialType, const char* filePath);

	void setPhongMat(const PhongMaterialData& phong);

	ComPtr<ID3D12Resource> getMaterialBuffer() const { return materialBuffer; }
	ModuleShaderDescriptors* getShaderDescriptors() const { return shaderDescriptors; }
	UINT getShaderDescriptorsIndex() const { return shaderDescriptorIndex; }
	Type getMaterialType() { return type; }
	const PhongMaterialData& getPhong() const { return materialData.phong; }

private:

	ComPtr<ID3D12Resource> colorTexture;
	ComPtr<ID3D12Resource> materialBuffer;
	UINT shaderDescriptorIndex = 0;

	struct 
	{
		MaterialData basic;
		PhongMaterialData phong;
	} materialData;

	Type type = BASIC;

	ModuleShaderDescriptors* shaderDescriptors = nullptr;

};

