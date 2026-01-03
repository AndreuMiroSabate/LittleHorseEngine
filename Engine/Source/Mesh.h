#pragma once

namespace tinygltf
{
	class Model;
	struct Mesh;
	struct Primitive;
}

class Mesh
{
public:
	struct Vertex
	{
		Vector3 position;
		Vector2 texCoord0;
	};

	void loadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);

	bool loadAccessorData(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, int accesorIndex)
	{
		const tinygltf::Accessor& accessor = model.accessors[accesorIndex];
		if (elemCount == accessor.count)
		{
			const tinygltf::BufferView& bufferView = model.bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = model.buffers[bufferView.buffer];
			size_t bufferOffset = bufferView.byteOffset + accessor.byteOffset;
			const uint8_t* srcData = buffer.data.data() + bufferOffset;
			for (size_t i = 0; i < elemCount; ++i)
			{
				memcpy(data + i * stride, srcData + i * accessor.ByteStride(bufferView), elemSize);
			}
			return true;
		}
		return false;
	}

	bool loadAccessorDataX(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, const std::map<std::string, int>& attributes, const char* accesorName)
	{
		const auto& it = attributes.find(accesorName);
		if (it != attributes.end())
		{
			return loadAccessorData(data, elemSize, stride, elemCount, model, it->second);
		}
		return false;
	}
private:
	std::string name;
	
	ComPtr<ID3D12Resource> vertexBuffer;
	ComPtr<ID3D12Resource> indexBuffer;

};

