#pragma once
class RenderTextureCustom
{

public:

	RenderTextureCustom(const char* name, UINT width, UINT height, DXGI_FORMAT format, DXGI_FORMAT depthFormat = DXGI_FORMAT_UNKNOWN, const Vector4& clearColor = Vector4::Zero)
		: name(name), format(format), depthFormat(depthFormat), clearColor(clearColor) 
	{};

	~RenderTextureCustom();

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

	DXGI_FORMAT format;
	DXGI_FORMAT depthFormat;

	Vector4 clearColor;

	const char* name;

private:
	void transitionToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState, D3D12_RESOURCE_STATES oldState);

	void setRenderTarget(ID3D12GraphicsCommandList* commandList);


};

