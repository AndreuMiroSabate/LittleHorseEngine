#pragma once
#include "Module.h"
#include "dxgi1_6.h"
#include "d3d12.h"
#include "ImGuiPass.h"
#include <cstdint>



class ModuleD3D12 :
    public Module
{
public:
	ModuleD3D12(HWND hWnd);
	~ModuleD3D12();

	bool init() override;
	void preRender() override;
	void postRender() override;
	void render() override;

	bool createDevice();
	void enbleDebugLayer();
	void createCommandQueue();
	void createCommandList();
	void createSwapChain();
	void createRenderTargets();
	void createdrawFence();
	void resize();

	D3D12_CPU_DESCRIPTOR_HANDLE getRenderTargetDescriptor();

	HWND getHwnd() { return hWnd; }
	ID3D12Device5* getDevice() { return device.Get(); }
	ID3D12GraphicsCommandList* getCommandList() { return comandList.Get(); }

	void getWindowSize(unsigned& width, unsigned& height);


private:
	HWND hWnd = NULL;

	ComPtr<IDXGIFactory6> factory;
	ComPtr<IDXGIAdapter4> adpater;
	ComPtr<ID3D12Device5> device;
	ComPtr<ID3D12InfoQueue> infoQueue;
	ComPtr<ID3D12CommandQueue> comandQueue;
	ComPtr<ID3D12GraphicsCommandList> comandList;
	ComPtr<ID3D12CommandAllocator> commandAllocator[FRAMES_IN_FLIGHT];
	ComPtr<IDXGISwapChain4> swapChain;
	ComPtr<ID3D12Resource> backBuffers[FRAMES_IN_FLIGHT];
	ComPtr<ID3D12DescriptorHeap> rtdescriptorHeap;
	ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	ComPtr<ID3D12Fence> drawFence;

	HANDLE drawFenceEvent = NULL;

	unsigned windowWidth = 0;
	unsigned windowHeight = 0;

	unsigned currentBackBufferIdx = 0;
	unsigned drawFenceValues[FRAMES_IN_FLIGHT] = { 0, 0, 0 };
	unsigned drawFenceCounter = 0;

	unsigned frameValues[FRAMES_IN_FLIGHT] = { 0, 0, 0 };
	unsigned frameIndex = 0;
	unsigned lastCompletedFrame = 0;
	
};

