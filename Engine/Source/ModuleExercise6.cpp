#include "Globals.h"
#include "ModuleExercise6.h"
#include "ModuleSamplers.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleCamara.h"
#include "ModuleRingBuffer.h"
#include "Application.h"
#include "ReadData.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include "d3dx12.h"



bool ModuleExercise6::init()
{
	createRootSignature();
	createPSO();
	ModuleD3D12* d3d12 = app->getD3D12();
	ModuleResources* resource = app->getResources();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();

	model = std::make_unique<Model>();
	model->LoadModel("Assets/Models/Duck/glTF/Duck.gltf", "Assets/Models/Duck/glTF/", BasicMaterial::PHONG);
	model->setModelMatrix(Matrix::CreateScale(0.01f, 0.01f, 0.01f));

	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());

	debugDrawPass = std::make_unique<DebugDrawPass>(d3d12->getDevice(), d3d12->getCommandQueue());
	imGuiPass = std::make_unique<ImGuiPass>(d3d12->getDevice(), d3d12->getHwnd());
	return true;
}

void ModuleExercise6::preRender()
{
	imGuiPass->startFrame();
	ImGuizmo::BeginFrame();

	unsigned width = app->getD3D12()->getWidth();
	unsigned height = app->getD3D12()->getHeight();

	ImGuizmo::SetRect(0, 0, float(width), float(height));
}

void ModuleExercise6::render()
{

	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12GraphicsCommandList* commandList = d3d12->getCommandList();

	ID3D12DescriptorHeap* srvHeap;
	ModuleSamplers* samples = app->getSamplers();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();
	ModuleRingBuffer* ringBuffer = app->getRingBuffer();

	commandsImGui();

	unsigned width, height;
	d3d12->getWindowSize(width, height);

	commandList->Reset(d3d12->getCommandAllocator(), pipelineState.Get());

	Matrix modelMatrix = model->getModelMatrix();

	Matrix view = app->getCamara()->GetViewMatrix();

	app->getCamara()->SetLookAt(Vector3::Zero);

	float aspectRatio = float(width) / float(height);
	float fovY = XM_PIDIV4;

	Matrix projection = Matrix::CreatePerspectiveFieldOfView(fovY, aspectRatio, 0.1f, 100.0f);

	Matrix mvp = (modelMatrix * view * projection).Transpose();


	float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };

	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		d3d12->getBackBuffers(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);
	commandList->ResourceBarrier(1, &barrier);



	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
	D3D12_RECT scissorRect{ 0, 0, (LONG)width, (LONG)height };
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3d12->getRenderTargetDescriptor();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3d12->getDepthStencilDescriptor();

	PerFrame perframe;
	perframe.L = light.L;
	perframe.Lc = light.Lc;
	perframe.Ac = light.Ac;
	perframe.viewPos = app->getCamara()->GetPos();

	perframe.L.Normalize();


	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->RSSetViewports(1, &viewport);
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

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

			PerInstance perInstance = { model->getModelMatrix().Transpose(), model->getNormalMatrix().Transpose(), material.getPhong()};

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
	imGuiPass->record(commandList, d3d12->getRenderTargetDescriptor());

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

bool ModuleExercise6::createRootSignature()
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

void ModuleExercise6::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	D3D12_INPUT_ELEMENT_DESC inputLayout[] = { {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											  {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											  {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

	auto dataVS = DX::ReadData(L"Exercise6VS.cso");
	auto dataPS = DX::ReadData(L"Exercise6PS.cso");

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

Matrix ModuleExercise6::createModelMatrix()
{
	float time = float(app->getElapsedMilis()) / 1000.0f;
	Matrix translation = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	Matrix rotation = Matrix::CreateRotationY(time);
	Matrix scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	return scale * rotation * translation;
}

void ModuleExercise6::commandsImGui()
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
			if (material.getMaterialType() == BasicMaterial::PHONG)
			{

				if (ImGui::CollapsingHeader("Mat", ImGuiTreeNodeFlags_DefaultOpen))
				{
					PhongMaterialData phong = material.getPhong();
					if (ImGui::ColorEdit3("Diffuse Colour", reinterpret_cast<float*>(&phong.diffuseColour)))
					{
						material.setPhongMat(phong);
					}

					bool hasTexture = phong.hasDiffuseTex;
					if (ImGui::Checkbox("Use Texture", &hasTexture))
					{
						phong.hasDiffuseTex = hasTexture;
						material.setPhongMat(phong);
					}

					if (ImGui::DragFloat("Kd", &phong.kDifusse, 0.01f))
					{
						material.setPhongMat(phong);
					}

					if (ImGui::DragFloat("Ks", &phong.kSpecular, 0.01f))
					{
						material.setPhongMat(phong);
					}

					if (ImGui::DragFloat("shininess", &phong.kShininess))
					{
						material.setPhongMat(phong);
					}
				}
			}
		}

		ImGui::End();
	}

	if (showGuizmo)
	{
		unsigned width = app->getD3D12()->getWidth();
		unsigned height = app->getD3D12()->getHeight();

		const Matrix& view = app->getCamara()->GetViewMatrix();
		Matrix projection = app->getCamara()->GetProjectionMatrix(float(width) / float(height));

		ImGuizmo::Manipulate((float*)&view,(float*)&projection,gizmoOperationLocal,ImGuizmo::LOCAL,(float*)&modelM);

		ImGuiIO& io = ImGui::GetIO();

		app->getCamara()->SetBlockMouse(io.WantCaptureMouse || (showGuizmo && ImGuizmo::IsUsing()));

		if (ImGuizmo::IsUsing())
		{
			model->setModelMatrix(modelM);
		}
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



