#include "Globals.h"
#include "ModuleExrcise7.h"
#include "ModuleSamplers.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleCamara.h"
#include "ModuleRingBuffer.h"
#include "Application.h"
#include "ReadData.h"
#include"RenderTextureCustom.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include "d3dx12.h"

bool ModuleExercise7::init()
{
	createRootSignature();
	createPSO();
	ModuleD3D12* d3d12 = app->getD3D12();
	ModuleResources* resource = app->getResources();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();

	model = std::make_unique<Model>();
	model->LoadModel("Assets/Models/Duck/glTF/Duck.gltf", "Assets/Models/Duck/glTF/", BasicMaterial::PBR_PHONG);
	model->setModelMatrix(Matrix::CreateScale(0.01f, 0.01f, 0.01f));

	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

	debugDrawPass = std::make_unique<DebugDrawPass>(d3d12->getDevice(), d3d12->getCommandQueue());
	imGuiPass = std::make_unique<ImGuiPass>(d3d12->getDevice(), d3d12->getHwnd());
	renderTexture = std::make_unique<RenderTextureCustom>("Render Texture", 800, 600, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_D32_FLOAT, Vector4::Zero);

	if(!renderTexture->init(app))
		return false;

	return true;
}

void ModuleExercise7::preRender()
{
	imGuiPass->startFrame();
	ImGuizmo::BeginFrame();

	ImGuiID dockspace_id = ImGui::GetID("MyDockNodeId");
	ImGui::DockSpaceOverViewport(dockspace_id);

	static bool init = true;
	ImVec2 mainSize = ImGui::GetMainViewport()->Size;
	if (init)
	{
		init = false;
		ImGui::DockBuilderRemoveNode(dockspace_id);
		ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_CentralNode);
		ImGui::DockBuilderSetNodeSize(dockspace_id, mainSize);

		ImGuiID dock_id_left = 0, dock_id_right = 0;
		ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Left, 0.75f, &dock_id_left, &dock_id_right);
		ImGui::DockBuilderDockWindow("Geometry Viewer Options", dock_id_right);
		ImGui::DockBuilderDockWindow("Scene", dock_id_left);

		ImGui::DockBuilderFinish(dockspace_id);
	}

	if (canvasSize.x > 0.0f && canvasSize.y > 0.0f)
	{
		renderTexture->resize(int(canvasSize.x), int(canvasSize.y));
	}

	ImGuizmo::SetRect(0, 0, (float)canvasSize.x, (float)canvasSize.y);
}

void ModuleExercise7::render()
{
	commandsImGui();

	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12GraphicsCommandList* commandList = d3d12->getCommandList();
	ID3D12DescriptorHeap* srvHeap;
	ModuleSamplers* samples = app->getSamplers();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();
	ModuleRingBuffer* ringBuffer = app->getRingBuffer();

	commandList->Reset(d3d12->getCommandAllocator(), pipelineState.Get());

	renderToTexture(commandList);

	float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	unsigned width, height;
	d3d12->getWindowSize(width, height);

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3d12->getBackBuffers(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &barrier);

	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
	D3D12_RECT scissorRect{ 0, 0, LONG(width), LONG(height) };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getRenderTargetDescriptor();
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	imGuiPass->record(commandList, rtvHandle);

	barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3d12->getBackBuffers(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	commandList->ResourceBarrier(1, &barrier);

	commandList->Close();
	ID3D12CommandList* commandLists[] = { commandList };
	d3d12->getCommandQueue()->ExecuteCommandLists(UINT(std::size(commandLists)), commandLists);
}

bool ModuleExercise7::createRootSignature()
{
	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	CD3DX12_ROOT_PARAMETER rootParameters[5] = {};
	CD3DX12_DESCRIPTOR_RANGE tableRange;
	CD3DX12_DESCRIPTOR_RANGE samplesRange;

	tableRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);
	samplesRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 4, 0);

	rootParameters[0].InitAsConstants((sizeof(Matrix) / sizeof(UINT32)), 0, 0, D3D12_SHADER_VISIBILITY_VERTEX);
	rootParameters[1].InitAsConstantBufferView(1, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[2].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_ALL);
	rootParameters[3].InitAsDescriptorTable(1, &tableRange, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[4].InitAsDescriptorTable(1, &samplesRange, D3D12_SHADER_VISIBILITY_PIXEL);

	rootSignatureDesc.Init(5, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> blob;

	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr)))
		return false;

	if (FAILED(app->getD3D12()->getDevice()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
		return false;

	return true;
}

void ModuleExercise7::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = { {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											  {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											  {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

	auto dataVS = DX::ReadData(L"Exercise7VS.cso");
	auto dataPS = DX::ReadData(L"Exercise7PS.cso");

	psoDesc.VS = { dataVS.data(), dataVS.size() };
	psoDesc.PS = { dataPS.data(), dataPS.size() };



	psoDesc.InputLayout = { inputLayout, sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	app->getD3D12()->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
}

Matrix ModuleExercise7::createModelMatrix()
{
	float time = float(app->getElapsedMilis()) / 1000.0f;
	Matrix translation = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	Matrix rotation = Matrix::CreateRotationY(time);
	Matrix scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	return scale * rotation * translation;
}

void ModuleExercise7::commandsImGui()
{
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("Windows"))
	{
		if (ImGui::MenuItem("Texture Viewer Options", NULL, geometryViewerOptionsOpen))
		{
			geometryViewerOptionsOpen = !geometryViewerOptionsOpen;
		}
		if (ImGui::MenuItem("FPS", NULL, fpsWindowOpen))
		{
			fpsWindowOpen = !fpsWindowOpen;
		}
		if (ImGui::MenuItem("Show Demo Window", NULL, showDemoWindow))
		{
			showDemoWindow = !showDemoWindow;
		}
		ImGui::EndMenu();
	}
	if (ImGui::BeginMenu("About"))
	{
		ImGui::Text("Little Horse Engine\n");
		ImGui::Text("This is a engine made by Andreu Miro \nfor the masters degree of \nadvanced programming of AAA videogames at UPC.");
		ImGui::Text("MIT License \nCopyright(c) 2025 AndreuMiroSabate\n Permission is hereby granted, free of charge, to any person obtaining a copy\n"
			"of this software and associated documentation files(the Software), to deal\n"
			"in the Software without restriction, including without limitation the rights\n"
			"to use, copy, modify, merge, publish, distribute, sublicense, and /or sell\n"
			"copies of the Software, and to permit persons to whom the Software is\n"
			"furnished to do so, subject to the following conditions :\n"

			"The above copyright notice and this permission notice shall be included in all\n"
			"copies or substantial portions of the Software.\n"

			"THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n"
			"IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n"
			"FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\n"
			"AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n"
			"LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n"
			"OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n"
			"SOFTWARE.");
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();


	Matrix modelM = model->getModelMatrix();
	static ImGuizmo::OPERATION gizmoOperationLocal = ImGuizmo::TRANSLATE;

	if (geometryViewerOptionsOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Geometry Viewer Options");
		ImGui::Checkbox("Show grid", &showGrid);
		ImGui::Checkbox("Show axis", &showAxis);
		ImGui::Checkbox("Show guizmo", &showGuizmo);




		if (ImGui::IsKeyPressed(ImGuiKey_E))
		{
			gizmoOperationLocal = ImGuizmo::TRANSLATE;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_R))
		{
			gizmoOperationLocal = ImGuizmo::ROTATE;
		}
		if (ImGui::IsKeyPressed(ImGuiKey_T))
		{
			gizmoOperationLocal = ImGuizmo::SCALE;
		}

		ImGui::RadioButton("Translate", (int*)&gizmoOperationLocal, (int)ImGuizmo::TRANSLATE);
		ImGui::SameLine();
		ImGui::RadioButton("Rotate", (int*)&gizmoOperationLocal, ImGuizmo::ROTATE);
		ImGui::SameLine();
		ImGui::RadioButton("Scale", (int*)&gizmoOperationLocal, ImGuizmo::SCALE);

		float translation[3], rotation[3], scale[3];
		ImGuizmo::DecomposeMatrixToComponents((float*)&modelM, translation, rotation, scale);
		bool transformed = ImGui::DragFloat3("Tranlated", translation, 0.01f);
		transformed |= ImGui::DragFloat3("Rotation", rotation, 0.1f);
		transformed |= ImGui::DragFloat3("Scale", scale, 0.01f);

		if (transformed)
		{
			ImGuizmo::RecomposeMatrixFromComponents(translation, rotation, scale, (float*)&modelM);
			model->setModelMatrix(modelM);
		}

		ImGui::Separator();

		ImGui::DragFloat3("Light Direction", reinterpret_cast<float*>(&light.L), 0.01f, -1.0f, 1.0f);
		ImGui::SameLine();
		if (ImGui::SmallButton("Normalize"))
		{
			light.L.Normalize();
		}
		ImGui::ColorEdit3("Light Colour", reinterpret_cast<float*>(&light.Lc), ImGuiColorEditFlags_NoAlpha);
		ImGui::ColorEdit3("Ambient Colour", reinterpret_cast<float*>(&light.Ac), ImGuiColorEditFlags_NoAlpha);
		for (BasicMaterial& material : model->GetMaterials())
		{
			if (material.getMaterialType() == BasicMaterial::PBR_PHONG)
			{

				if (ImGui::CollapsingHeader("Mat", ImGuiTreeNodeFlags_DefaultOpen))
				{
					PBRPhongMaterialData pbrPhong = material.getPBRPhong();
					if (ImGui::ColorEdit3("Diffuse Colour", reinterpret_cast<float*>(&pbrPhong.diffuseColour)))
					{
						material.setPBRPhongMat(pbrPhong);
					}

					bool hasTexture = pbrPhong.hasDiffuseTex;
					if (ImGui::Checkbox("Use Texture", &hasTexture))
					{
						pbrPhong.hasDiffuseTex = hasTexture;
						material.setPBRPhongMat(pbrPhong);
					}

					if (ImGui::ColorEdit3("Specular", reinterpret_cast<float*>(&pbrPhong.kSpecular), 0.01f))
					{
						material.setPBRPhongMat(pbrPhong);
					}

					if (ImGui::DragFloat("shininess", &pbrPhong.kShininess))
					{
						material.setPBRPhongMat(pbrPhong);
					}
				}
			}
		}

		ImGui::End();
	}

	bool viewerFocused = false;
	ImGui::Begin("Scene");
	const char* frameName = "Scene Frame";
	ImGuiID id(10);

	ImVec2 max = ImGui::GetWindowContentRegionMax();
	ImVec2 min = ImGui::GetWindowContentRegionMin();
	canvasPos = min;
	canvasSize = ImVec2(max.x - min.x, max.y - min.y);
	ImVec2 cursorPos = ImGui::GetCursorScreenPos();

	ImGui::BeginChildFrame(id, canvasSize, ImGuiWindowFlags_NoScrollbar);
	viewerFocused = ImGui::IsWindowFocused();

	ImGui::Image((ImTextureID)renderTexture->getSRVGPUHandle().ptr, canvasSize);

	if (showGuizmo)
	{
		const Matrix& viewMatrix = app->getCamara()->GetViewMatrix();
		Matrix projMatrix = app->getCamara()->GetProjectionMatrix(float(canvasSize.x) / float(canvasSize.y));

		// Manipulate the object
		ImGuizmo::SetRect(cursorPos.x, cursorPos.y, canvasSize.x, canvasSize.y);
		ImGuizmo::SetDrawlist();
		ImGuizmo::Manipulate((const float*)&viewMatrix, (const float*)&projMatrix, gizmoOperationLocal, ImGuizmo::LOCAL, (float*)&modelM);
	}

	ImGui::EndChildFrame();
	ImGui::End();

	ImGuiIO& io = ImGui::GetIO();

	//app->getCamara()->SetBlockMouse(!viewerFocused);

	if (ImGuizmo::IsUsing())
	{
		model->setModelMatrix(modelM);
	}


	if (fpsWindowOpen)
	{
		ImGui::Begin("FPS");
		ImGui::PlotHistogram("Framerate", [](void* data, int idx) {
			Application* app = reinterpret_cast<Application*>(data);
			static const int MAX_FPS_TICKS = 30;
			static std::array<float, MAX_FPS_TICKS> frameTimes = {};
			static int currentIndex = 0;
			frameTimes[currentIndex] = app->getFPS();
			currentIndex = (currentIndex + 1) % MAX_FPS_TICKS;
			return frameTimes[idx];
			}, app, 30, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
		ImGui::Text("FPS: %.2f", app->getFPS());
		ImGui::PlotHistogram("Frame Time (ms)", [](void* data, int idx) {
			Application* app = reinterpret_cast<Application*>(data);
			static const int MAX_FPS_TICKS = 30;
			static std::array<float, MAX_FPS_TICKS> frameTimes = {};
			static int currentIndex = 0;
			frameTimes[currentIndex] = app->getAvgElapsedMs();
			currentIndex = (currentIndex + 1) % MAX_FPS_TICKS;
			return frameTimes[idx];
			}, app, 30, 0, nullptr, 0.0f, 100.0f, ImVec2(0, 80));
		ImGui::Text("dt:%.4f", ImGui::GetIO().DeltaTime);
		ImGui::End();
	}

	if (showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}


}

void ModuleExercise7::renderToTexture(ID3D12GraphicsCommandList* commandList)
{

	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12DescriptorHeap* srvHeap;
	ModuleSamplers* samples = app->getSamplers();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();
	ModuleRingBuffer* ringBuffer = app->getRingBuffer();

	unsigned width = renderTexture->getWidth();
	unsigned height = renderTexture->getHeight();

	float aspectRatio = float(width) / float(height);
	float fovY = XM_PIDIV4;

	Matrix modelMatrix = model->getModelMatrix();

	Matrix view = app->getCamara()->GetViewMatrix();

	Matrix projection = Matrix::CreatePerspectiveFieldOfView(fovY, aspectRatio, 0.1f, 100.0f);
	Matrix mvp = (modelMatrix * view * projection).Transpose();

	/*D3D12_VIEWPORT viewport{ 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
	D3D12_RECT scissorRect{ 0, 0, (LONG)width, (LONG)height };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getRenderTargetDescriptor();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3d12->getDepthStencilDescriptor();*/

	PerFrame perframe;
	perframe.L = light.L;
	perframe.Lc = light.Lc;
	perframe.Ac = light.Ac;
	perframe.viewPos = app->getCamara()->GetPos();

	perframe.L.Normalize();

	renderTexture->beginRender(commandList);

	commandList->SetGraphicsRootSignature(rootSignature.Get());

	ID3D12DescriptorHeap* descriptorHeaps[] = { shaderDescriptors->getDescriptorHeap(), samples->getHeap() };

	commandList->SetDescriptorHeaps(2, descriptorHeaps);

	commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / sizeof(UINT32), &mvp, 0);
	commandList->SetGraphicsRootConstantBufferView(1, ringBuffer->allocBufferAcess(&perframe));
	commandList->SetGraphicsRootDescriptorTable(4, samples->GetGPUHandle(samplerIndex));

	BEGIN_EVENT(commandList, "Model Render Pass");
	for (const Mesh& mesh : model->GetMeshes())
	{
		if (mesh.getMaterialIndex() < model->GetMaterials().size())
		{
			const BasicMaterial& material = model->GetMaterials()[mesh.getMaterialIndex()];

			PerInstance perInstance = { model->getModelMatrix().Transpose(), model->getNormalMatrix().Transpose(), material.getPBRPhong() };

			commandList->SetGraphicsRootConstantBufferView(2, ringBuffer->allocBufferAcess(&perInstance));
			commandList->SetGraphicsRootDescriptorTable(3, material.getShaderDescriptors()->getGPUHandle(material.getShaderDescriptorsIndex()));

			mesh.drawIndexes(commandList);
		}
	}
	END_EVENT(commandList);

	if (showGrid)
	{
		dd::xzSquareGrid(-10.0f, 10.0f, 0.0f, 1.0f, dd::colors::Gray);
	}
	if (showAxis)
	{
		dd::axisTriad(ddConvert(Matrix::Identity), 0.1f, 1.0f);
	}

	debugDrawPass->record(commandList, width, height, view, projection);
	renderTexture->endRender(commandList);

}



