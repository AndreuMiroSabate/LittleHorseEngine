#include "Globals.h"
#include "ModuleExercice4.h"
#include "ModuleSamplers.h"
#include "ModuleShaderDescriptors.h"
#include "ModuleCamara.h"
#include "Application.h"
#include "ReadData.h"

#include <d3d12.h>
#include <d3dcompiler.h>
#include "d3dx12.h"



bool ModuleExercice4::init()
{
	createVertexBuffer();
	createRootSignature();
	createPSO();
	ModuleD3D12* d3d12 = app->getD3D12();
	ModuleResources* resource = app->getResources();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();
	
	dogTexture = resource->createTextureFromFile(L"Assets/Textures/dog.dds");
	dogTextureDescriptorIndex = shaderDescriptors->allocteDescriptor(); //The allocation of a descriptor needs to be revvised and redo, now doesn't work properly
	shaderDescriptors->createSRV(dogTexture.Get(), 0);  //For this exercise is good, but an scalable version is needed for more textures

	debugDrawPass = std::make_unique<DebugDrawPass>(d3d12->getDevice(), d3d12->getCommandQueue());
	imGuiPass = std::make_unique<ImGuiPass>(d3d12->getDevice(), d3d12->getHwnd());
	return true;
}

void ModuleExercice4::preRender()
{
	imGuiPass->startFrame();
}

void ModuleExercice4::render()
{

	ModuleD3D12* d3d12 = app->getD3D12();
	ID3D12GraphicsCommandList* commandList = d3d12->getCommandList();

	ID3D12DescriptorHeap* srvHeap;
	ModuleSamplers* samples = app->getSamplers();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();

	commandsImGui();

	unsigned width, height;
	d3d12->getWindowSize(width, height);

	commandList->Reset(d3d12->getCommandAllocator(), pipelineState.Get());

	Matrix model = createModelMatrix();

	Matrix view = app->getCamara()->GetViewMatrix();

	app->getCamara()->SetLookAt(Vector3::Zero);

	float aspectRatio = float(width) / float(height);
	float fovY = XM_PIDIV4;

	Matrix projection = Matrix::CreatePerspectiveFieldOfView(fovY, aspectRatio, 0.1f, 100.0f);

	Matrix mvp = (model * view * projection).Transpose();


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

	
	commandList->RSSetScissorRects(1, &scissorRect);
	commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	commandList->RSSetViewports(1, &viewport);
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);

	ID3D12DescriptorHeap* descriptorHeaps[] = { shaderDescriptors->getDescriptorHeap(), samples->getHeap()};
	commandList->SetDescriptorHeaps(2, descriptorHeaps);

	commandList->SetGraphicsRoot32BitConstants(0, sizeof(XMMATRIX) / sizeof(UINT32), &mvp, 0);
	commandList->SetGraphicsRootDescriptorTable(1, shaderDescriptors->getDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
	commandList->SetGraphicsRootDescriptorTable(2, samples->GetGPUHandle(samplerIndex));

	commandList->DrawInstanced(6, 1, 0, 0);


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

void ModuleExercice4::createVertexBuffer()
{
	struct Vertex
	{
		Vector3 position;
		Vector2 uv;
	};
	static Vertex vertices[6] =
	{
	 { Vector3(-1.0f, -1.0f, 0.0f), Vector2(-0.2f, 1.2f) },
	 { Vector3(-1.0f, 1.0f, 0.0f), Vector2(-0.2f, -0.2f) },
	 { Vector3(1.0f, 1.0f, 0.0f), Vector2(1.2f, -0.2f) },
	 { Vector3(-1.0f, -1.0f, 0.0f), Vector2(-0.2f, 1.2f) },
	 { Vector3(1.0f, 1.0f, 0.0f), Vector2(1.2f, -0.2f) },
	 { Vector3(1.0f, -1.0f, 0.0f), Vector2(1.2f, 1.2f) }
	};


	vertexBuffer = app->getResources()->CreateDefaultBuffer(vertices, sizeof(vertices), "Exercice4");

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.SizeInBytes = sizeof(vertices);
	vertexBufferView.StrideInBytes = sizeof(Vertex);

}

bool ModuleExercice4::createRootSignature()
{
	/*CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
	CD3DX12_ROOT_PARAMETER rootParameters[3];

	rootParameters[0].InitAsConstants(sizeof(Matrix) / sizeof(UINT32), 0);

	rootSignatureDesc.Init(1, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);*/

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	CD3DX12_ROOT_PARAMETER rootParameters[3] = {};
	CD3DX12_DESCRIPTOR_RANGE tableRange;
	CD3DX12_DESCRIPTOR_RANGE samplesRange;

	tableRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	samplesRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, 2, 0);

	rootParameters[0].InitAsConstants((sizeof(Matrix) / sizeof(UINT32)), 0, 0, D3D12_SHADER_VISIBILITY_VERTEX); 
	rootParameters[1].InitAsDescriptorTable(1, &tableRange, D3D12_SHADER_VISIBILITY_PIXEL);
	rootParameters[2].InitAsDescriptorTable(1, &samplesRange, D3D12_SHADER_VISIBILITY_PIXEL);

	rootSignatureDesc.Init(3, rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> blob;

	if (FAILED(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, nullptr)))
		return false;

	if (FAILED(app->getD3D12()->getDevice()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&rootSignature))))
		return false;

	return true;
}

void ModuleExercice4::createPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	

	auto dataVS = DX::ReadData(L"Exercise4VS.cso");
	auto dataPS = DX::ReadData(L"Exercise4PS.cso");

	psoDesc.VS = { dataVS.data(), dataVS.size() };
	psoDesc.PS = { dataPS.data(), dataPS.size() };

	D3D12_INPUT_ELEMENT_DESC inputLayout[] = { {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
											  {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0} };

	psoDesc.InputLayout = { inputLayout, sizeof(inputLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.NumRenderTargets = 1;
	psoDesc.SampleDesc = { 1, 0 };
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

	app->getD3D12()->getDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));

}

Matrix ModuleExercice4::createModelMatrix()
{
	float time = float(app->getElapsedMilis()) / 1000.0f;
	Matrix translation = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
	Matrix rotation = Matrix::CreateRotationY(time);
	Matrix scale = Matrix::CreateScale(1.0f, 1.0f, 1.0f);
	return scale * rotation * translation;
}

void ModuleExercice4::commandsImGui()
{
	ImGui::BeginMainMenuBar();
	if(ImGui::BeginMenu("Windows"))
	{
		if (ImGui::MenuItem("Texture Viewer Options", NULL, textureViewerOptionsOpen))
		{
			textureViewerOptionsOpen = !textureViewerOptionsOpen;
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

	if(textureViewerOptionsOpen)
	{
		ImGui::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
		ImGui::Begin("Texture Viewer Options");
		ImGui::Checkbox("Show grid", &showGrid);
		ImGui::Checkbox("Show axis", &showAxis);
		ImGui::Combo("Sampler", &samplerIndex, "Linear/Wrap\0Point/Wrap\0Linear/Clamp\0Point/Clamp", 4);
		ImGui::End();
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

	if(showDemoWindow)
	{
		ImGui::ShowDemoWindow(&showDemoWindow);
	}

}

