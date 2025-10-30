#pragma once
#include "Module.h"
#include "ImGuiPass.h"
#include "ModuleD3D12.h"
#include "ImGuiPass.h"

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

};

