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
	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12Device2* device = d3d12->getDevice();


	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = NUM_DESCRIPTORS* DESCRIPTORS_PER_TABLE;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;

	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap));

	heap->SetName(L"Shader Descriptor Heap");

	cpuHandle = heap->GetCPUDescriptorHandleForHeapStart();
	gpuHandle = heap->GetGPUDescriptorHandleForHeapStart();

	for (uint32_t i = 0; i < descriptorSize; i++)
		freeHandles.push_back(i);

	return true;
}


void ModuleShaderDescriptors::createSRV(ID3D12Resource* resource, UINT slot)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = resource->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = resource->GetDesc().MipLevels;
	srvDesc.Texture2D.MipLevels = 1;

	if (resource)
	{
		app->getD3D12()->getDevice()->CreateShaderResourceView(
			resource,
			&srvDesc,
			this->getCPUHandle(slot));
	}
}

UINT ModuleShaderDescriptors::createNullTexture2DSRV()
{
	UINT index = allocteDescriptor();
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	app->getD3D12()->getDevice()->CreateShaderResourceView(nullptr, &srvDesc, this->getCPUHandle(index));

	return index;
}


UINT ModuleShaderDescriptors::allocteDescriptor()
{
	assert(!freeHandles.empty());
	UINT index = freeHandles.back();
	freeHandles.pop_back();
	return index;
}

void ModuleShaderDescriptors::freeDescriptor(UINT index)
{
	freeHandles.push_back(index);
}

D3D12_CPU_DESCRIPTOR_HANDLE ModuleShaderDescriptors::getCPUHandle(UINT index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handle = heap->GetCPUDescriptorHandleForHeapStart();
	handle.ptr += index * app->getD3D12()->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
	return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE ModuleShaderDescriptors::getGPUHandle(UINT index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handle = heap->GetGPUDescriptorHandleForHeapStart();
	handle.ptr += index * app->getD3D12()->getDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	return handle;
}

void ModuleShaderDescriptors::resetDescriptors()
{
	freeHandles.clear();
	for (uint32_t i = 0; i < NUM_DESCRIPTORS * DESCRIPTORS_PER_TABLE; i++)
		freeHandles.push_back(i);
}

