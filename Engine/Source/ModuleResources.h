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

    void CreateUploadBuffer();
    void CreateDefaultBuffer();

private:

    ModuleD3D12* d3d12 = nullptr;
};

