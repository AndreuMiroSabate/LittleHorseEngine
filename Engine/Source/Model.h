#pragma once
#include <vector>
#include <span>
#include "Mesh.h"
#include "BasicMaterial.h"
class Model
{

public:

	std::unique_ptr<Mesh[]> meshes;
	//std::vector<Nodes> nodes;
	//std::vector<Animation> animations;
	std::unique_ptr<BasicMaterial[]> materials;

	void LoadModel(const char* fileName, const char* basePath);
	void setModelMatrix(const Matrix& matrix) { mMatrix = matrix; }
	Matrix& getModelMatrix() { return mMatrix; }

	std::span<const Mesh> GetMeshes() const { return std::span<const Mesh>(meshes.get(), meshCount); }
	std::span<const BasicMaterial> GetMaterials() const { return std::span<const BasicMaterial>(materials.get(), materialCount); }

private:

	uint32_t meshCount = 0;
	uint32_t materialCount = 0;

	Matrix mMatrix = Matrix::Identity;

};

