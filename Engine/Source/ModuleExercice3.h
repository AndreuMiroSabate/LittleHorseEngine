#pragma once
#include "Module.h"
#include "ModuleResources.h"
#include "DebugDrawPass.h"

class ModuleExercice3 :
    public Module
{
public:
    bool init() override;
    void render() override;

    void createVertexBuffer();
    bool createRootSignature();
    void createPSO();

    Matrix createModelMatrix();

private:

    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    ComPtr<ID3D12PipelineState> pipelineState;
    ComPtr<ID3D12RootSignature> rootSignature;
    std::unique_ptr<DebugDrawPass> debugDrawPass;
    
};

