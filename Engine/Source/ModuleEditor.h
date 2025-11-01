#pragma once
#include "Module.h"
#include "ImGuiPass.h"
#include "ModuleD3D12.h"
#include "ImGuiPass.h"

class ModuleD3D12;
class ImGuiPass;

class ModuleEditor :
    public Module
{
public:
    ModuleEditor();
    ~ModuleEditor();

    bool postInit() override;
    void preRender() override;
    void render() override;

private:

	/*ImGuiPass* imguiPass = nullptr;
	ModuleD3D12* d3d12 = nullptr;*/
    
};

