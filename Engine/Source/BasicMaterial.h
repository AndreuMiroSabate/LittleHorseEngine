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

struct PBRPhongMaterialData
{
	XMFLOAT3 diffuseColour;
	BOOL hasDiffuseTex;
	XMFLOAT3 kSpecular;
	float kShininess;
	
};

class BasicMaterial
{

public: 

	enum Type
	{
		BASIC = 0,
		PHONG,
		PBR_PHONG
	};

public:

	BasicMaterial();
	~BasicMaterial();

	void load(const tinygltf::Model& model, const tinygltf::Material& material, Type materialType, const char* filePath);

	void setPhongMat(const PhongMaterialData& phong);
	void setPBRPhongMat(const PBRPhongMaterialData& pbrPhong);

	ComPtr<ID3D12Resource> getMaterialBuffer() const { return materialBuffer; }
	ModuleShaderDescriptors* getShaderDescriptors() const { return shaderDescriptors; }
	UINT getShaderDescriptorsIndex() const { return shaderDescriptorIndex; }
	Type getMaterialType() { return type; }
	const PhongMaterialData& getPhong() const { return materialData.phong; }
	const PBRPhongMaterialData& getPBRPhong() const { return materialData.pbrPhong; }

private:

	ComPtr<ID3D12Resource> colorTexture;
	ComPtr<ID3D12Resource> materialBuffer;
	UINT shaderDescriptorIndex = 0;

	struct 
	{
		MaterialData basic;
		PhongMaterialData phong;
		PBRPhongMaterialData pbrPhong;
	} materialData;

	Type type = BASIC;

	ModuleShaderDescriptors* shaderDescriptors = nullptr;

};

