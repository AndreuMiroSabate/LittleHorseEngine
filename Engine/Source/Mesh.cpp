#include "Globals.h"
#include "Mesh.h"


#include "tiny_gltf.h"

void Mesh::loadMesh(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive)
{
	const auto& itPos = primitive.attributes.find("POSITION");

	if (itPos != primitive.attributes.end())
	{
		uint32_t numVertices = uint32_t(model.accessors[itPos->second].count);
		Vertex* vertices = new Vertex[numVertices];
		uint8_t* vertexData = (uint8_t*)vertices;
		loadAccessorData(vertexData + offsetof(Vertex, position), sizeof(Vector3), sizeof(Vertex), numVertices, model, itPos->second);
		loadAccessorDataX(vertexData + offsetof(Vertex, texCoord0), sizeof(Vector2), sizeof(Vertex), numVertices, model, primitive.attributes, "TEXCOORD_0");
	}

	const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];

	if(indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT || 
	   indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT ||
	   indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
	{
		uint32_t indexEementSize = tinygltf::GetComponentSizeInBytes(indexAccessor.componentType);
		uint32_t numIndices = uint32_t(indexAccessor.count);

		uint8_t* indices = new uint8_t[numIndices * indexEementSize];
		loadAccessorData(indices, indexEementSize, indexEementSize, numIndices, model, primitive.indices);

	}
}
