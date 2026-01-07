#include "Globals.h"
#include "ModuleRingBuffer.h"

#include "Application.h"
#include "ModuleD3D12.h"

#define MEMORY_TOTAL_SIZE 10 * (1 << 20)

ModuleRingBuffer::ModuleRingBuffer()
{

}

ModuleRingBuffer::~ModuleRingBuffer()
{

}

bool ModuleRingBuffer::init()
{
	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12Device5* device = d3d12->getDevice();
	ID3D12CommandQueue* queue = d3d12->getCommandQueue();

	size = alignUp(MEMORY_TOTAL_SIZE, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&fixedBuffer));
	fixedBuffer->SetName(L"RingBuffer");

	CD3DX12_RANGE readRange(0, 0);
	fixedBuffer->Map(0, &readRange, bufferData);

	return true;
}
