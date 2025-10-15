#include "Globals.h"
#include "ModuleD3D12.h"


ModuleD3D12::ModuleD3D12(HWND hWnd) : hWnd(hWnd)
{
}
ModuleD3D12::~ModuleD3D12()
{
}

bool ModuleD3D12::init()
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
return true;
}

bool ModuleD3D12::crateDevice()
{
#if defined(_DEBUG)
	CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory));
#else
	CreateDXGIFactory2(0, IID_PPV_ARGS(&factory));
#endif

	ComPtr<IDXGIAdapter4> adpater;
	ComPtr<ID3D12Device> device;
	factory->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adpater));
	D3D12CreateDevice(adpater.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));

	ComPtr<ID3D12InfoQueue> infoQueue;
	device.As(&infoQueue);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);

	return true;
}
