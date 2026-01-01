#pragma once
#include "Module.h"
#include "ModuleResources.h"
#include "ImGuiPass.h"
#include "DebugDrawPass.h"
class ModuleExercise5 :
    public Module
{
public:
    bool init() override;
    void preRender() override;
    void render() override;

    void createVertexBuffer();
    bool createRootSignature();
    void createPSO();

    Matrix createModelMatrix();

    void commandsImGui();

private:

    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    ComPtr<ID3D12PipelineState> pipelineState;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12Resource> dogTexture;
    UINT dogTextureDescriptorIndex;
    std::unique_ptr<DebugDrawPass> debugDrawPass;
    std::unique_ptr<ImGuiPass> imGuiPass;

    bool showGrid = true;
    bool showAxis = true;

    bool fpsWindowOpen = true;
    bool textureViewerOptionsOpen = true;
    bool showDemoWindow = false;
    bool consoleOpen = true;

    int samplerIndex = 0;
};

