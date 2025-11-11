#include "Globals.h"
#include "ModuleExercice2.h"
#include "Application.h"
#include "ReadData.h"



bool ModuleExercice2::init()
{
	createVertexBuffer();
	createRootSignature();
	createPSO();
	return true;
}

void ModuleExercice2::render()
{
	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12GraphicsCommandList* commandList = d3d12->getCommandList();

	unsigned width, height;

	commandList->Reset(d3d12->getCommandAllocator(), pipelineState.Get());

	float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3d12->getBackBuffers(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &barrier);

	d3d12->getWindowSize(width, height);

	D3D12_VIEWPORT viewport{0.0,0.0, float(width), float(height), 0.0, 1.0};
	D3D12_RECT scissorRect{ 0, 0, (LONG)width, (LONG)height };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getRenderTargetDescriptor();

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->DrawInstanced(3, 1, 0, 0);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3d12->getBackBuffers(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();
	ID3D12CommandList* commandLists[] = { commandList };
	d3d12->getCommandQueue()->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList* const*>(commandList));
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
	psoDesc.pRootSignature = rootSignature.Get();

	auto dataVS = DX::ReadData(L"Exercice2VS.cso");
	auto dataPS = DX::ReadData(L"Exercise2PS.cso");

	psoDesc.VS = { dataVS.data(), dataVS.size() };
	psoDesc.PS = { dataPS.data(), dataPS.size() };

	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	psoDesc.InputLayout = { inputLayout, sizeof(inputLayout)/ sizeof(D3D12_INPUT_ELEMENT_DESC)};

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	app->getD3D12()->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

}
