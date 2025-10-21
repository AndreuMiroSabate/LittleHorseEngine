#pragma once
#include "Module.h"
#include "dxgi1_6.h"


class ModuleD3D12 :
    public Module
{
public:
	ModuleD3D12(HWND hWnd);
	~ModuleD3D12();

	bool init() override;

	bool createDevice();
	bool enbleDebugLayer();

private:

	ComPtr<IDXGIFactory6> factory;
};

