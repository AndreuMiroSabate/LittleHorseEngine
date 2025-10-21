#pragma once
#include "Module.h"
#include "dxgi1_6.h"
#include "d3d12.h"
#include <cstdint>



class ModuleD3D12 :
    public Module
{
public:
	ModuleD3D12(HWND hWnd);
	~ModuleD3D12();

	bool init() override;
	void preRender() override;
	void render() override;

	bool createDevice();
	bool enbleDebugLayer();
	bool createCommandQueue();
	bool createCommandList();
	bool createSwapChain();

private:

	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adpater;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12InfoQueue> infoQueue;
	ComPtr<ID3D12CommandQueue> comandQueue;
	ComPtr<ID3D12CommandList> comandList;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	
};

