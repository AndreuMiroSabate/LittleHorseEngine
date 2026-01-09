#pragma once
#include "Module.h"
#include "dxgi1_6.h"
#include "d3d12.h"

class ModuleRenderTarget :
    public Module
{

public:
    ModuleRenderTarget();
    ~ModuleRenderTarget();

    bool init() override;
    void preRender() override;

	void createRenderTarget(UINT width, UINT height, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	//void createDepthStencil(UINT width, UINT height);

	//void beginScene(ID3D12GraphicsCommandList* commandList);
	//void endScene(ID3D12GraphicsCommandList* commandList);

	ID3D12Resource* getTexture() const {return texture.Get();}

	D3D12_GPU_DESCRIPTOR_HANDLE getGPUHandle() const { return gpuHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE getRTVHandle() const { return rtvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE getDSVHandle() const { return dsvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE getSRVHandle() const { return srvHandle; }


	//void resize(UINT newWidth, UINT newHeight);
	//void releaseResources();


private:

	ComPtr<ID3D12Resource> texture;
	ComPtr<ID3D12Resource> depthStencil;
	ComPtr<ID3D12Device> device;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle;

	UINT width = 0;
	UINT height = 0;

	DXGI_FORMAT format;

	bool needsResize = false;

};

