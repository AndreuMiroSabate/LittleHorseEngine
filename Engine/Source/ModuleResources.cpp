#include "Globals.h"
#include "ModuleResources.h"
#include "Application.h"

ModuleResources::ModuleResources()
{

}
ModuleResources::~ModuleResources()
{

}

bool ModuleResources::init()
{
	d3d12 = app->getD3D12();
}

void ModuleResources::CreateUploadBuffer()
{
	ComPtr<ID3D12Device5> device;
	ComPtr<ID3D12Resource> buffer;

	device = d3d12->getDevice();

	//D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//device->CreateCommittedResource1(&uploadHeap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer))
	// Map the buffer: get a CPU pointer to its memory
	//BYTE* pData = nullptr;
	//CD3DX12_RANGE readRange(0, 0); // We won't read from it, so range is (0,0)
	//buffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));
	// Copy our application data into the GPU buffer
	//memcpy(pData, cpuData, dataSize);
	// Unmap the buffer (invalidate the pointer)
	//buffer->Unmap(0, nullptr);
}

void ModuleResources::CreateDefaultBuffer()
{
	ComPtr<ID3D12Device5> device;
	ComPtr<ID3D12Resource> buffer;

	device = d3d12->getDevice();

	//D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	//device->CreateCommittedResource1(&uploadHeap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&buffer))
	// 
	// heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	// Map the buffer: get a CPU pointer to its memory
	//BYTE* pData = nullptr;
	//CD3DX12_RANGE readRange(0, 0); // We won't read from it, so range is (0,0)
	//buffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));
	// Copy our application data into the GPU buffer
	//memcpy(pData, cpuData, dataSize);
	// Unmap the buffer (invalidate the pointer)
	//buffer->Unmap(0, nullptr);
	d3d12->flush();
}