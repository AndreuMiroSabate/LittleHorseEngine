#pragma once
#include "Module.h"
class ModuleRingBuffer :
    public Module
{
public:

    ModuleRingBuffer();
    ~ModuleRingBuffer();

    bool init() override;

private:

    ComPtr<ID3D12Resource> fixedBuffer;
    size_t size = 0;

    void** bufferData = nullptr;

};

