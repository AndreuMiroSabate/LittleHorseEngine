#include "Globals.h"

#include "Application.h"
#include "ModuleEditor.h"

ModuleEditor::ModuleEditor()
{
	
}

ModuleEditor::~ModuleEditor()
{
	
}

bool ModuleEditor::postInit()
{
	d3d12 = app->getD3D12();
	imguiPass = new ImGuiPass(d3d12->getDevice(), d3d12->getHwnd(), D3D12_CPU_DESCRIPTOR_HANDLE{}, D3D12_GPU_DESCRIPTOR_HANDLE{});
	return true;
}

void ModuleEditor::preRender()
{
	imguiPass->startFrame();
	ImGui::Text("Hello, Editor!");
}

void ModuleEditor::render()
{
	imguiPass->record(d3d12->getCommandList(), d3d12->getRenderTargetDescriptor());
}

void ModuleEditor::consoleLog(const char* msg)
{
	ImGui::TextUnformatted(msg);
}
