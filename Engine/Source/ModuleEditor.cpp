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
}

void ModuleEditor::render()
{
	draw();
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
	ImGui::Checkbox("Show Demo Window", &showDemoWindow);

		if (ImGui::BeginMenu("Trial"))
		{
			ImGui::MenuItem("(demo menu)", NULL, false, false);
			if (ImGui::MenuItem("Gui Demo"))
			{
				showDemoWindow = !showDemoWindow;
			}
			if (ImGui::MenuItem("Documentation"))
			{
				ImGui::Text("Ha");
				//app->requestBrowser("https://github.com/AndreuMiroSabate/DirectX12Engine");
			}
			if (ImGui::MenuItem("Download latest"))
			{
				ImGui::Text("Ha");
				//app->requestBrowser("https://github.com/AndreuMiroSabate/DirectX12Engine");
			}
			if (ImGui::MenuItem("Report a bug"))
			{
				ImGui::Text("Ha");
				//app->requestBrowser("https://github.com/AndreuMiroSabate/DirectX12Engine");
			}
			ImGui::EndMenu();
	}
	ImGui::End();
	
}

void ModuleEditor::consoleLog(const char* msg)
{
	ImGui::TextUnformatted(msg);
}

void ModuleEditor::mainMenuBar()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				PostQuitMessage(0);
			}
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Edit"))
		{
			ImGui::EndMenu();
		}
		if(ImGui::BeginMenu("Windows"))
		{
			if (ImGui::MenuItem("Gui Demo"))
			{
				showDemoWindow = !showDemoWindow;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("About"))
		{
			ImGui::Text("DirectX12 Engine");
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
