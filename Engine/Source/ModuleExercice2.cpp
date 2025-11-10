#include "Globals.h"
#include "ModuleExercice2.h"
#include "Application.h"



bool ModuleExercice2::init()
{
	createVertexBuffer();
	createRootSignature();
	createPSO();
	return true;
}

void ModuleExercice2::createVertexBuffer()
{
	struct Vertex
	{
		float x, y, z;
	};

	static Vertex vertices[3] =
	{
		{-1.0f, -1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, -1.0f, 0.0f }
	};

	vertexBuffer = app->getResources()->CreateDefaultBuffer(vertices, sizeof(vertices), "Exercice2");

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(Vertex);
	vertexBufferView.StrideInBytes = sizeof(vertices);

}

bool ModuleExercice2::createRootSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	ComPtr<ID3DBlob> blob;

	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr)))
		return false;

	ComPtr<ID3D12RootSignature> rootSignature;
	if (FAILED(app->getD3D12()->getDevice()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
		return false;

	return true;
}

void ModuleExercice2::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	app->getD3D12()->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

}
