#include "Globals.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleD3D12.h"
#include "Application.h"

ModuleShaderDescriptors::ModuleShaderDescriptors()
{
}

ModuleShaderDescriptors::~ModuleShaderDescriptors()
{
}

bool ModuleShaderDescriptors::init()
{
	createSRV();
	return true;
}



void ModuleShaderDescriptors::createSRV()
{
	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12Device2* device = d3d12->getDevice();


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	heap->SetName(L"Shader Descriptor Heap");

	cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
	gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();
}
