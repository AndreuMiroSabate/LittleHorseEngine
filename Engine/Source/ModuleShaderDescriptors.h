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
    void createSRV(ID3D12Resource *resource, UINT8 slot);
	UINT createNullTexture2DSRV();
    //void allocateDescriptor();

    D3D12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT index);
    D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle(UINT index);

	UINT allocteDescriptor();

    void freeDescriptor(UINT index);

	void resetDescriptors();

    //void resetDescriptorHeap();

private:
    
    ComPtr<ID3D12DescriptorHeap> heap;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;

	std::vector<UINT> freeHandles;
	UINT descriptorSize = 4096;

    enum { NUM_DESCRIPTORS = 4096, DESCRIPTORS_PER_TABLE = 8 };

};

