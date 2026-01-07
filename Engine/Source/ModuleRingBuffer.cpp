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

	totalSize = alignUp(MEMORY_TOTAL_SIZE, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);

	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&fixedBuffer));
	fixedBuffer->SetName(L"RingBuffer");

	CD3DX12_RANGE readRange(0, 0);
	fixedBuffer->Map(0, &readRange, bufferData);

	currentIndex = d3d12->getBackBufferIndex();

	for (int i = 0; i < FRAMES_IN_FLIGHT; ++i)
	{
		memoryPerFrame[i] = 0;
	}

	return true;
}

void ModuleRingBuffer::preRender()
{
	ModuleD3D12* d3d12 = app->getD3D12();

	unsigned currentIndex = d3d12->getBackBufferIndex();

	tail = (tail + memoryPerFrame[currentIndex]) % totalSize;
	totalSize -= memoryPerFrame[currentIndex];

	memoryPerFrame[currentIndex] = 0;
}

D3D12_GPU_VIRTUAL_ADDRESS ModuleRingBuffer::allocBuffer(size_t size, const void* data)
{
	size = alignUp(size, D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);

	D3D12_GPU_VIRTUAL_ADDRESS adress = 0;;

	if (tail > head)
	{
		size_t available = tail - head;

		if (size <= available)
		{
			memcpy(bufferData + head, data, size);
			adress = fixedBuffer->GetGPUVirtualAddress() + head;
			head += size;
			return adress;
			
		}
		return 0;
	}

	if (tail < head)
	{
		size_t availableToEnd = totalSize - head;

		if (size <= availableToEnd)
		{
			memcpy(bufferData + head, data, size);
			adress = fixedBuffer->GetGPUVirtualAddress() + head;
			head += size;
			return adress;
		}
		else
		{
			head = 0;
		}

		if (size <= tail)
		{
			memcpy(bufferData + head, data, size);
			adress = fixedBuffer->GetGPUVirtualAddress() + head;
			head += size;
			return adress;
		}
		return 0;
	}

	if (tail == head)
	{
		if (size <= totalSize)
		{
			memcpy(bufferData, data, size);
			adress = fixedBuffer->GetGPUVirtualAddress();
			head = size;
			return adress;
		}
		return 0;
	}
	return 0;
}
