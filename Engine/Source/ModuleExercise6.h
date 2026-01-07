#pragma once
#include "Module.h"
#include "ModuleResources.h"
#include "ImGuiPass.h"
#include "DebugDrawPass.h"
#include "Model.h"
#include "Mesh.h"
#include "ImGuizmo.h"
#include "BasicMaterial.h"


class ModuleExercise6 :
    public Module
{
    struct PerInstance
    {
        Matrix modelMat;
        Matrix normalMat;

        PhongMaterialData material;
    };

    struct PerFrame
    {
        Vector3 L = Vector3::UnitX;
        float pad0;
        Vector3 Lc = Vector3::One;
        float pad1;
        Vector3 Ac = Vector3::Zero;
        float pad2;
        Vector3 viewPos = Vector3::Zero;
        float pad3;

    };

    struct Light
    {
        Vector3 L = Vector3::One * (-0.5f);
        Vector3 Lc = Vector3::One;
        Vector3 Ac = Vector3::One * (0.1f);
    };

public:
    bool init() override;
    void preRender() override;
    void render() override;

    bool createRootSignature();
    void createPSO();

    Matrix createModelMatrix();

    void commandsImGui();

private:

    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    ComPtr<ID3D12PipelineState> pipelineState;
    ComPtr<ID3D12RootSignature> rootSignature;
    std::unique_ptr<Model> model;
    std::unique_ptr<DebugDrawPass> debugDrawPass;
    std::unique_ptr<ImGuiPass> imGuiPass;

    Light light;

    ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;

    bool showGrid = true;
    bool showAxis = true;
    bool showGuizmo = true;

    bool fpsWindowOpen = true;
    bool geometryViewerOptionsOpen = true;
    bool showDemoWindow = false;
    bool consoleOpen = true;

    int samplerIndex = 0;
};

