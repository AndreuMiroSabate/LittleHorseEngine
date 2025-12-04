#include "Globals.h"
#include "ModuleResources.h"
#include "Application.h"
#include "DirectXTex.h"

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

ComPtr<ID3D12Resource> ModuleResources::createTextureFromFile(const wchar_t* filePath)
{
	DirectX::ScratchImage image;
	if (FAILED(LoadFromDDSFile(filePath, DDS_FLAGS_NONE, nullptr, image)))
	{
		if (FAILED(LoadFromTGAFile(filePath, nullptr, image)))
		{
			LoadFromWICFile(filePath, WIC_FLAGS_NONE, nullptr, image);
		}
	}

	DirectX::TexMetadata metaData = image.GetMetadata();

	D3D12_RESOURCE_DESC descTex = CD3DX12_RESOURCE_DESC::Tex2D(metaData.format, UINT64(metaData.width),
		UINT(metaData.height), UINT16(metaData.arraySize),
		UINT16(metaData.mipLevels));

	CD3DX12_HEAP_PROPERTIES heap(D3D12_HEAP_TYPE_DEFAULT);
	ComPtr<ID3D12Resource> texture;
	device->CreateCommittedResource(&heap, D3D12_HEAP_FLAG_NONE, &descTex,
		D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
		IID_PPV_ARGS(&texture));

	ComPtr<ID3D12Resource> stagingBuffer;

	UINT64 size = GetRequiredIntermediateSize(texture.Get(), 0, image.GetImageCount());
	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(size);
	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&stagingBuffer));

	std::vector<D3D12_SUBRESOURCE_DATA> subData;
	subData.reserve(image.GetImageCount());

	for (size_t item = 0; item < metaData.arraySize; ++item)
	{
		for (size_t level = 0; level < metaData.mipLevels; ++level)
		{
			const DirectX::Image* subImg = image.GetImage(level, item, 0);
			D3D12_SUBRESOURCE_DATA data = { subImg->pixels, subImg->rowPitch, subImg->slicePitch };
			subData.push_back(data);
		}
	}
	UpdateSubresources(commandList.Get(), texture.Get(), stagingBuffer.Get(), 0, 0, UINT(image.GetImageCount()), subData.data());

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	commandList->ResourceBarrier(1, &barrier);
	commandList->Close();

	ID3D12CommandList* commandLists[] = { commandList.Get() };
	ID3D12CommandQueue* queue = d3d12->getCommandQueue();

	queue->ExecuteCommandLists(UINT(std::size(commandLists)), commandLists);

	d3d12->flush();

	commandAllocator->Reset();
	commandList->Reset(commandAllocator.Get(), nullptr);

	texture->SetName(filePath);
	return texture;

}