#include "Globals.h"
#include "ModuleExercice4.h"
#include "Application.h"
#include "ReadData.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include "d3dx12.h"



bool ModuleExercice4::init()
{
	createVertexBuffer();
	createRootSignature();
	createPSO();
	ModuleD3D12* d3d12 = app->getD3D12();
	debugDrawPass = std::make_unique<DebugDrawPass>(d3d12->getDevice(), d3d12->getCommandQueue());
	return true;
}

void ModuleExercice4::render()
{
	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12GraphicsCommandList* commandList = d3d12->getCommandList();

	unsigned width, height;
	d3d12->getWindowSize(width, height);

	commandList->Reset(d3d12->getCommandAllocator(), pipelineState.Get());

	Matrix model = createModelMatrix();
	Matrix view = Matrix::CreateLookAt(
		Vector3(0.0f, 10.0f, 10.0f),
		Vector3::Zero,
		Vector3::Up
	);

	float aspectRatio = float(width) / float(height);
	float fovY = XM_PIDIV4;

	Matrix projection = Matrix::CreatePerspectiveFieldOfView(fovY, aspectRatio, 0.1f, 100.0f);

	Matrix mvp = (model * view * projection).Transpose();


	float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3d12->getBackBuffers(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &barrier);



	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
	D3D12_RECT scissorRect{ 0, 0, (LONG)width, (LONG)height };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getRenderTargetDescriptor();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3d12->getDepthStencilDescriptor();

	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / sizeof(UINT32), &mvp, 0);

	commandList->DrawInstanced(6, 1, 0, 0);

	dd::xzSquareGrid(-10.0f, 10.0f, 0.0f, 1.0f, dd::colors::Gray);
	dd::axisTriad(ddConvert(Matrix::Identity), 0.1f, 1.0f);

	debugDrawPass->record(commandList, width, height, view, projection);



	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3d12->getBackBuffers(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();
	ID3D12CommandList* commandLists[] = { commandList };
	d3d12->getCommandQueue()->ExecuteCommandLists(UINT(std::size(commandLists)), commandLists);
}

void ModuleExercice4::createVertexBuffer()
{
	struct Vertex
	{
		Vector3 position;
		Vector2 uv;
	};
	static Vertex vertices[6] =
	{
	 { Vector3(-1.0f, -1.0f, 0.0f), Vector2(-0.2f, 1.2f) },
	 { Vector3(-1.0f, 1.0f, 0.0f), Vector2(-0.2f, -0.2f) },
	 { Vector3(1.0f, 1.0f, 0.0f), Vector2(1.2f, -0.2f) },
	 { Vector3(-1.0f, -1.0f, 0.0f), Vector2(-0.2f, 1.2f) },
	 { Vector3(1.0f, 1.0f, 0.0f), Vector2(1.2f, -0.2f) },
	 { Vector3(1.0f, -1.0f, 0.0f), Vector2(1.2f, 1.2f) }
	};


	vertexBuffer = app->getResources()->CreateDefaultBuffer(vertices, sizeof(vertices), "Exercice4");

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(vertices);
	vertexBufferView.StrideInBytes = sizeof(Vertex);

}

bool ModuleExercice4::createRootSignature()
{
	/*CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	CD3DX12_ROOT_PARAMETER rootParameters[3];

	rootParameters[0].InitAsConstants(sizeof(Matrix) / sizeof(UINT32), 0);

	rootSignatureDesc.Init(1, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);*/

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	CD3DX12_ROOT_PARAMETER rootParameters[2];
	CD3DX12_DESCRIPTOR_RANGE tableRange;
	tableRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	rootParameters[0].InitAsConstants((sizeof(Matrix) / sizeof(UINT32)), 0, 0, D3D12_SHADER_VISIBILITY_VERTEX); 
	rootParameters[1].InitAsDescriptorTable(1, &tableRange, D3D12_SHADER_VISIBILITY_PIXEL); 
	rootSignatureDesc.Init(2, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> blob;

	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr)))
		return false;

	if (FAILED(app->getD3D12()->getDevice()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
		return false;

	return true;
}

void ModuleExercice4::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature.Get();

	auto dataVS = DX::ReadData(L"Exercise4VS.cso");
	auto dataPS = DX::ReadData(L"Exercise4PS.cso");

	psoDesc.VS = { dataVS.data(), dataVS.size() };
	psoDesc.PS = { dataPS.data(), dataPS.size() };

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = { {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											  {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

	psoDesc.InputLayout = { inputLayout, sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC) };

	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	app->getD3D12()->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

}

Matrix ModuleExercice4::createModelMatrix()
{
	float time = float(app->getElapsedMilis()) / 1000.0f;
	Matrix translation = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	Matrix rotation = Matrix::CreateRotationY(time);
	Matrix scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	return scale * rotation * translation;
}

