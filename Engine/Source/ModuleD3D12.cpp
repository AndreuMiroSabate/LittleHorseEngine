#include "Globals.h"
#include "ModuleD3D12.h"


ModuleD3D12::ModuleD3D12(HWND hWnd)
{
}
ModuleD3D12::~ModuleD3D12()
{
}

bool ModuleD3D12::init()
{
	enbleDebugLayer();
	createDevice();
	createCommandList();
	createCommandQueue();

	return true;
}

void ModuleD3D12::preRender()
{
	
}

void ModuleD3D12::render()
{
	
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
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&comandList));
	
}
void ModuleD3D12::createSwapChain()
{
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

	//Need to be changed
	swapChainDesc.Width = 0;
	swapChainDesc.Height = 0;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1,0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;

	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	swapChainDesc.Flags = 0;

	factory->CreateSwapChainForHwnd(comandQueue.Get(), hWnd, &swapChainDesc, nullptr, nullptr, &swapChain);

}
void ModuleD3D12::resourceBarrier()
{
	CD3DX12_RESOURCE_BARRIER transitionBarrier;
	transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
		nullptr,
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET
	);

	//D3D12_RESOURCE_BARRIER barrier = {};

	//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;	
	//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//barrier.Transition.pResource = nullptr; //Need to be changed

	comandList->ResourceBarrier(1, &transitionBarrier);
}
