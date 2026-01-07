#pragma once

#include <string>
#include <map>

namespace tinygltf { class Model;  struct Mesh; struct Primitive; struct BufferView; struct Buffer; }

class Mesh
{
public:
	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord0;
		Vector3 normal = Vector3::UnitZ;
		Vector3 tangent = Vector3::UnitX;
	};

	Mesh();
	~Mesh();

	void loadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);

	void drawIndexes(ID3D12GraphicsCommandList* commandList) const;

	bool loadAccessorData(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, int accesorIndex);

	bool loadAccessorDataX(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, const std::map<std::string, int>& attributes, const char* accesorName);

	int getMaterialIndex() const { return materialIndex; }

private:
	std::string name;

	uint32_t indexElementSize = 0;
	uint32_t numIndices = 0;
	uint32_t numVertices = 0;

	int materialIndex = -1;
	
	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

};

