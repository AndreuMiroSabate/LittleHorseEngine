#pragma once
#include "Application.h"

class RenderTextureCustom
{

public:

	RenderTextureCustom(const char* name, UINT width, UINT height, DXGI_FORMAT format, DXGI_FORMAT depthFormat, const Vector4& clearColor)
		: name(name), width(width), height(height), format(format), depthFormat(depthFormat), clearColor(clearColor) 
	{};

	~RenderTextureCustom();

	bool init(Application* app);

	void cleanUp();

	UINT getWidth() const { return width; }
	UINT getHeight() const { return height; }
	D3D12_CPU_DESCRIPTOR_HANDLE getRTV() const { return rtvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE getSRV() const { return srvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE getDSV() const { return dsvHandle; }

	void beginRender(ID3D12GraphicsCommandList* commandList);
	void endRender(ID3D12GraphicsCommandList* commandList);

	void resize(UINT newWidth, UINT newHeight);


private:

	UINT width = 0;
	UINT height = 0;

	ComPtr<ID3D12Resource> texture;
	ComPtr<ID3D12Resource> depthStencil;

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {};
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = {};
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = {};

	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;

	DXGI_FORMAT format;
	DXGI_FORMAT depthFormat;

	Vector4 clearColor;

	Application* app = nullptr;

	const char* name;

private:
	void transitionToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState, D3D12_RESOURCE_STATES oldState);

	void setRenderTarget(ID3D12GraphicsCommandList* commandList);


};

