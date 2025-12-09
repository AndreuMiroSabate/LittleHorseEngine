#pragma once
#include "Module.h"
class ModuleShaderDescriptors :
    public Module
{
public:
    ModuleShaderDescriptors();
    ~ModuleShaderDescriptors();

    bool init() override;
    //void preRender() override;

    ID3D12DescriptorHeap* getDescriptorHeap() { return heap.Get(); }
    void createSRV();
    //void allocateDescriptor();

    D3D12_CPU_DESCRIPTOR_HANDLE* getCPUHandle() { return &cpuHandle; }
    D3D12_GPU_DESCRIPTOR_HANDLE* getGPUHandle() { return &gpuHandle; }

    //void resetDescriptorHeap();

private:
    
    ComPtr<ID3D12DescriptorHeap> heap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

};

