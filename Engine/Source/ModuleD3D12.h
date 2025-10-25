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
	void enbleDebugLayer();
	void createCommandQueue();
	void createCommandList();
	void createSwapChain();
	void resourceBarrier();

private:
	HWND hWnd;

	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adpater;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12InfoQueue> infoQueue;
	ComPtr<ID3D12CommandQueue> comandQueue;
	ComPtr<ID3D12GraphicsCommandList> comandList;
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<IDXGISwapChain1> swapChain;
	
};

