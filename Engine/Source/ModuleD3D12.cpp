#include "Globals.h"
#include "ModuleD3D12.h"


ModuleD3D12::ModuleD3D12(HWND hWnd): 	hWnd(hWnd)
{

}
ModuleD3D12::~ModuleD3D12()
{
	comandQueue->Signal(drawFence.Get(), ++drawFenceCounter);
	drawFence->SetEventOnCompletion(drawFenceCounter, drawFenceEvent);
	WaitForSingleObject(drawFenceEvent, INFINITE);
}

bool ModuleD3D12::init()
{
	getWindowSize(windowWidth, windowHeight);
	enbleDebugLayer();
	createDevice();
	createCommandQueue();
	createSwapChain();
	createRenderTargets();
	createCommandList();
	createdrawFence();
	
	

	currentBackBufferIdx = swapChain->GetCurrentBackBufferIndex();

	return true;
}

void ModuleD3D12::preRender()
{
	currentBackBufferIdx = swapChain->GetCurrentBackBufferIndex();
	if(drawFenceValues[currentBackBufferIdx] != 0)
	{
		
		drawFence->SetEventOnCompletion(drawFenceValues[currentBackBufferIdx], drawFenceEvent);
		WaitForSingleObject(drawFenceEvent, INFINITE);
		drawFenceValues[currentBackBufferIdx];
		lastCompletedFrame = std::max(frameValues[currentBackBufferIdx], lastCompletedFrame);
	}
	frameIndex++;
	frameValues[currentBackBufferIdx] = frameIndex;

	commandAllocator[currentBackBufferIdx]->Reset();
}
void ModuleD3D12::postRender()
{
	swapChain->Present(1, 0);
	drawFenceValues[currentBackBufferIdx] = ++drawFenceCounter;
	comandQueue->Signal(drawFence.Get(), drawFenceValues[currentBackBufferIdx]);
}

void ModuleD3D12::render()
{
	comandList->Reset(commandAllocator[currentBackBufferIdx].Get(), nullptr);
	CD3DX12_RESOURCE_BARRIER transitionBarrier;
	transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers[currentBackBufferIdx].Get(),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	comandList->ResourceBarrier(1, &transitionBarrier);
	float clearColor[] = { 0.0f, 1.0f, 0.7f, 1.0f };
	comandList->ClearRenderTargetView(getRenderTargetDescriptor(), clearColor, 0, nullptr);

	transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		backBuffers[currentBackBufferIdx].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT
	);
	comandList->ResourceBarrier(1, &transitionBarrier);
	comandList->Close();
	ID3D12CommandList* commandsLists[] = { comandList.Get() };
	comandQueue->ExecuteCommandLists(UINT(std::size(commandsLists)), commandsLists);
	
}

void ModuleD3D12::enbleDebugLayer()
{
#if defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
		{
			debugController->EnableDebugLayer();
		}
	}
#endif
}

bool ModuleD3D12::createDevice()
{
#if defined(_DEBUG)
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
	CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
#endif

	
	factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adpater));
	D3D12CreateDevice(adpater.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

	
	device.As(&infoQueue);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

	return true;
}

void ModuleD3D12::createCommandQueue()
{
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	device->CreateCommandQueue(&desc, IID_PPV_ARGS(&comandQueue));
}

void ModuleD3D12::createCommandList()
{
	for (unsigned int i = 0; i < FRAMES_IN_FLIGHT; ++i)
	{
		device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator[i]));
	}
	
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(&comandList));
	comandList->Close();
}
void ModuleD3D12::createSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	
	//Need to be changed
	swapChainDesc.Width = windowWidth;
	swapChainDesc.Height = windowHeight;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1,0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = FRAMES_IN_FLIGHT;

	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0;
	
	ComPtr<IDXGISwapChain1> tempSwapChain;

	factory->CreateSwapChainForHwnd(comandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain);
	tempSwapChain.As(&swapChain);
	factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

}

D3D12_CPU_DESCRIPTOR_HANDLE ModuleD3D12::getRenderTargetDescriptor()
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(rtdescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
										 currentBackBufferIdx,
										 device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
}

void ModuleD3D12::getWindowSize(unsigned& width, unsigned& height)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	width = rect.right - rect.left;
	height = rect.bottom - rect.top;


}

void ModuleD3D12::createRenderTargets()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FRAMES_IN_FLIGHT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtdescriptorHeap));
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtdescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (unsigned int i = 0; i < FRAMES_IN_FLIGHT; ++i)
	{
		swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		device->CreateRenderTargetView(backBuffers[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
}

void ModuleD3D12::createdrawFence()
{
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&drawFence));

	drawFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
}
