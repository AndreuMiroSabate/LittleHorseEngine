#include "Globals.h"
#include "RenderTextureCustom.h"

#include "ModuleD3D12.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleShaderDescriptors.h"

#include <d3d12.h>

RenderTextureCustom::~RenderTextureCustom()
{
	cleanUp();
}

void RenderTextureCustom::cleanUp()
{
	texture.Reset();
	depthStencil.Reset();
	rtvHeap.Reset();
	dsvHeap.Reset();
}

bool RenderTextureCustom::init(Application* app)
{
	if(!app)
		return false;
	this->app = app;

    ModuleResources* resources = app->getResources();
    ModuleD3D12* d3d12 = app->getD3D12();
    ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();

    if (!resources || !d3d12 || !shaderDescriptors)
        return false;

    texture = resources->createRenderTarget(width, height, format, clearColor, name);
    if (!texture) return false;

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 1;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	d3d12->getDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

    rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();

    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Format = format;
    rtvDesc.Texture2D.MipSlice = 0;
    rtvDesc.Texture2D.PlaneSlice = 0;

    d3d12->getDevice()->CreateRenderTargetView(texture.Get(), &rtvDesc, rtvHandle);

    UINT srvIndex = shaderDescriptors->allocteDescriptor();
    srvHandle = shaderDescriptors->getCPUHandle(srvIndex);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Format = format;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.PlaneSlice = 0;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

    d3d12->getDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, srvHandle);

    if (depthFormat != DXGI_FORMAT_UNKNOWN)
    {
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.NumDescriptors = 1;
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		d3d12->getDevice()->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

        dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();

        D3D12_CLEAR_VALUE depthClearValue = {};
        depthClearValue.Format = depthFormat;
        depthClearValue.DepthStencil.Depth = 1.0f;
        depthClearValue.DepthStencil.Stencil = 0;

        D3D12_RESOURCE_DESC depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(depthFormat, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

        d3d12->getDevice()->CreateCommittedResource(&heapProps,D3D12_HEAP_FLAG_NONE,&depthDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &depthClearValue, IID_PPV_ARGS(&depthStencil));

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Format = depthFormat;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.Texture2D.MipSlice = 0;

        d3d12->getDevice()->CreateDepthStencilView(depthStencil.Get(), &dsvDesc, dsvHandle);
    }

	shaderDescriptors->createSRV(texture.Get(), 0);

    return true;
}

void RenderTextureCustom::beginRender(ID3D12GraphicsCommandList* commandList)
{
	transitionToState(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	setRenderTarget(commandList);
	
	commandList->ClearRenderTargetView(rtvHandle, (float*)&clearColor, 0, nullptr);

	if (depthFormat != DXGI_FORMAT_UNKNOWN)
	{
		float clearDepth = 1.0f;
		UINT8 clearStencil = 0;
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, clearDepth, clearStencil, 0, nullptr);
	}
	//commandList->ClearRenderTargetView(rtvHandle, (float*)&clearColor, 0, nullptr);
}

void RenderTextureCustom::endRender(ID3D12GraphicsCommandList* commandList)
{
	transitionToState(commandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void RenderTextureCustom::transitionToState(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_STATES newState, D3D12_RESOURCE_STATES oldState)
{
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		texture.Get(),
		oldState,
		newState
	);
	commandList->ResourceBarrier(1, &barrier);
}

void RenderTextureCustom::setRenderTarget(ID3D12GraphicsCommandList* commandList)
{
	ModuleD3D12* d3d12 = app->getD3D12();

	if(depthFormat == DXGI_FORMAT_UNKNOWN)
	{
		commandList->OMSetRenderTargets(1, &rtvHandle, false, nullptr);
		commandList->ClearRenderTargetView(rtvHandle, (float*)&clearColor, 0, nullptr);
	}
	else
	{
		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);
		commandList->ClearRenderTargetView(rtvHandle, (float*)&clearColor, 0, nullptr);
		float clearDepth = 1.0f;
		UINT8 clearStencil = 0;
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, clearDepth, clearStencil, 0, nullptr);
	}

	D3D12_VIEWPORT viewport{ 0.0f, 0.0f, float(width), float(height), 0.0f, 1.0f };
	D3D12_RECT scissorRect{ 0, 0, (LONG)width, (LONG)height };
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);

}

void RenderTextureCustom::resize(UINT newWidth, UINT newHeight)
{
	if(newWidth == this->width && newHeight == this->height)
		return;

    if(!app)
		return;

	cleanUp();

	this->width = newWidth;
	this->height = newHeight;

	init(app);
}
