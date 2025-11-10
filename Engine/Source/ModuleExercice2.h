#pragma once
#include "Module.h"
#include "ModuleResources.h"
class ModuleExercice2 :
    public Module
{
public:
    bool init() override;

    void createVertexBuffer();
    bool createRootSignature();
    void createPSO();

private:

    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    ComPtr<ID3D12PipelineState> pipelineState;
};

