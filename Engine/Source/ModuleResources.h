#pragma once
#include "Module.h"
#include "ModuleD3D12.h"

class ModuleD3D12;

class ModuleResources :
    public Module
{
public:
    ModuleResources();
    ~ModuleResources();

    bool init() override;

    ComPtr<ID3D12Resource> CreateUploadBuffer(const void* data, size_t size, const char* name);
    ComPtr<ID3D12Resource> CreateDefaultBuffer(const void* data, size_t size, const char* name);
    ComPtr<ID3D12Resource> getUploadHeap(size_t size);
    ComPtr<ID3D12Resource> createTextureFromFile(const wchar_t* filename);

private:

    ModuleD3D12* d3d12 = nullptr;
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ComPtr<ID3D12Device5> device;
};

