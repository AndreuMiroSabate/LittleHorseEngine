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
