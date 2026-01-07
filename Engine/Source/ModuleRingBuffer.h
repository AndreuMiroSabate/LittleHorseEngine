#pragma once
#include "Module.h"
class ModuleRingBuffer :
    public Module
{
public:

    ModuleRingBuffer();
    ~ModuleRingBuffer();

    bool init() override;
    void preRender() override;

    

    D3D12_GPU_VIRTUAL_ADDRESS allocBuffer(size_t size, const void* data);

    template<typename T>
    D3D12_GPU_VIRTUAL_ADDRESS allocBufferAcess(const T* data)
    {
        return allocBuffer(alignUp(sizeof(T), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT), data);
    }


private:

    ComPtr<ID3D12Resource> fixedBuffer;
    size_t totalSize = 0;
    size_t tail = 0;
    size_t head = 0;
    size_t memoryPerFrame[FRAMES_IN_FLIGHT];
    unsigned currentIndex = 0;

    char* bufferData = nullptr;

};

