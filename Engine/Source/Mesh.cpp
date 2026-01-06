#include "Globals.h"
#include "Mesh.h"

#include "Application.h"
#include "ModuleResources.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE

#include "tiny_gltf.h"

#include <string>
#include <map>

Mesh::Mesh()
{
}
Mesh::~Mesh()
{
	vertexBuffer.Reset();
	indexBuffer.Reset();
}

void Mesh::loadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
	const auto& itPos = primitive.attributes.find("POSITION");

	ModuleResources* resources = app->getResources();
	Vertex* vertices = nullptr;
	uint8_t* indices = nullptr;

	if (itPos != primitive.attributes.end())
	{
		numVertices = uint32_t(model.accessors[itPos->second].count);
		vertices = new Vertex[numVertices];
		uint8_t* vertexData = (uint8_t*)vertices;
		loadAccessorData(vertexData + offsetof(Vertex, position), sizeof(Vector3), sizeof(Vertex), numVertices, model, itPos->second);
		loadAccessorDataX(vertexData + offsetof(Vertex, texCoord0), sizeof(Vector2), sizeof(Vertex), numVertices, model, primitive.attributes, "TEXCOORD_0");


		const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

		if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT ||
			indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT ||
			indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
		{
			indexElementSize = tinygltf::GetComponentSizeInBytes(indexAccessor.componentType);
			numIndices = uint32_t(indexAccessor.count);

			indices = new uint8_t[numIndices * indexElementSize];
			loadAccessorData(indices, indexElementSize, indexElementSize, numIndices, model, primitive.indices);

		}

		vertexBuffer = resources->CreateDefaultBuffer(vertices, sizeof(Vertex) * numVertices, "Mesh Vertex Buffer");
		indexBuffer = resources->CreateDefaultBuffer(indices, indexElementSize * numIndices, "Mesh Index Buffer");

		delete[] vertices;
		delete[] indices;

		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		vertexBufferView.SizeInBytes = sizeof(Vertex) * numVertices;

		static const DXGI_FORMAT indexFormats[] = {
			DXGI_FORMAT_R8_UINT,
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32_UINT
		};

		indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
		indexBufferView.Format = indexFormats[indexElementSize >> 1];
		indexBufferView.SizeInBytes = indexElementSize * numIndices;
	}
}

bool Mesh::loadAccessorData(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, int accesorIndex)
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

bool Mesh::loadAccessorDataX(uint8_t* data, size_t elemSize, size_t stride, size_t elemCount, const tinygltf::Model& model, const std::map<std::string, int>& attributes, const char* accesorName)
{
	const auto& it = attributes.find(accesorName);
	if (it != attributes.end())
	{
		return loadAccessorData(data, elemSize, stride, elemCount, model, it->second);
	}
	return false;
}
