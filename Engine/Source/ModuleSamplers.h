#pragma once
#include "Module.h"
class ModuleSamplers :
    public Module
{
public:
    
    bool init() override;


    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(int index) {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(samplerHeap->GetCPUDescriptorHandleForHeapStart(), index,
            samplerDescriptorSize);
    }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(int index) {
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(samplerHeap->GetGPUDescriptorHandleForHeapStart(), index,
            samplerDescriptorSize);
    }

    void CreateDefaultSamplers(ID3D12Device* device);

    ID3D12DescriptorHeap* getHeap() { return samplerHeap.Get(); }

private:
    ComPtr<ID3D12DescriptorHeap> samplerHeap;
    UINT samplerDescriptorSize;
	static const int MAX_SAMPLERS = 16;

};

