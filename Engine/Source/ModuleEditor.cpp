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
	imguiPass = new ImGuiPass(d3d12->getDevice(), d3d12->getHwnd(), d3d12->getRenderTargetDescriptor(), D3D12_GPU_DESCRIPTOR_HANDLE{});
	return true;
}

void ModuleEditor::preRender()
{
	imguiPass->startFrame();
	ImGui::ShowDemoWindow(&showDemoWindow);
	draw();
	//ImGui::Text("Hello, Editor!");
}

void ModuleEditor::render()
{
	imguiPass->record(d3d12->getCommandList(), d3d12->getRenderTargetDescriptor());
	//draw();
}

bool ModuleEditor::cleanUp()
{
	imguiPass->~ImGuiPass();
	return true;
}

void ModuleEditor::draw()
{
	ImGui::Begin("Menu");
	if (ImGui::BeginMenuBar())
	{

		if (ImGui::BeginMenu("Trial"))
		{
			ImGui::MenuItem("(demo menu)", NULL, false, false);
			if (ImGui::MenuItem("Gui Demo"))
			{
				showDemoWindow = !showDemoWindow;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				//app->requestBrowser("https://github.com/AndreuMiroSabate/DirectX12Engine");
			}
			if (ImGui::MenuItem("Download latest"))
			{
				//app->requestBrowser("https://github.com/AndreuMiroSabate/DirectX12Engine");
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				//app->requestBrowser("https://github.com/AndreuMiroSabate/DirectX12Engine");
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	ImGui::End();
	
}

void ModuleEditor::consoleLog(const char* msg)
{
	ImGui::TextUnformatted(msg);
}
