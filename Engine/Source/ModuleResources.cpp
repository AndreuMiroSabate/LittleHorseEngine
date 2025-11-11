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
	device = d3d12->getDevice();
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&commandList));
	commandList->Reset(commandAllocator.Get(), nullptr);

	return true;
}

ComPtr<ID3D12Resource> ModuleResources::CreateUploadBuffer(const void* data, size_t size, const char* name)
{
	ComPtr<ID3D12Resource> buffer;
	
	

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));
	
	BYTE* pData = nullptr;
	CD3DX12_RANGE readRange(0, 0); 
	buffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));
	memcpy(pData, data, size);
	buffer->Unmap(0, nullptr);

	return buffer;
}

ComPtr<ID3D12Resource>  ModuleResources::CreateDefaultBuffer(const void* data, size_t size, const char* name)
{
	ComPtr<ID3D12Resource> buffer;
	ComPtr<ID3D12CommandQueue> commandQueue;

	commandQueue = d3d12->getCommandQueue();

	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&buffer));
	
	heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	ComPtr<ID3D12Resource> uploadBuffer = getUploadHeap(size);

	BYTE* pData = nullptr;
	CD3DX12_RANGE readRange(0, 0); 
	uploadBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pData));
	memcpy(pData, data, size);
	
	uploadBuffer->Unmap(0, nullptr);
	commandList->CopyBufferRegion(buffer.Get(), 0, uploadBuffer.Get(), 0, size);
	commandList->Close();

	ID3D12CommandList* commandsLists[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(UINT(std::size(commandsLists)), commandsLists);

	d3d12->flush();

	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);

	return buffer;
}

ComPtr<ID3D12Resource> ModuleResources::getUploadHeap(size_t size)
{
	ComPtr<ID3D12Resource> buffer;
	D3D12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	CD3DX12_HEAP_PROPERTIES uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&buffer));
	return buffer;
}