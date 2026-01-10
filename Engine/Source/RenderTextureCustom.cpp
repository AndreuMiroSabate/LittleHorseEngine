#include "Globals.h"
#include "RenderTextureCustom.h"

#include "ModuleD3D12.h"
#include "Application.h"
#include "ModuleResources.h"
#include "ModuleShaderDescriptors.h"

#include <d3d12.h>

RenderTextureCustom::~RenderTextureCustom()
{

}

void RenderTextureCustom::beginRender(ID3D12GraphicsCommandList* commandList)
{
	transitionToState(commandList, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	setRenderTarget(commandList);
	if (depthFormat != DXGI_FORMAT_UNKNOWN)
	{
		float clearDepth = 1.0f;
		UINT8 clearStencil = 0;
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, clearDepth, clearStencil, 0, nullptr);
	}
	commandList->ClearRenderTargetView(rtvHandle, (float*)&clearColor, 0, nullptr);
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

	this->width = newWidth;
	this->height = newHeight;

	ModuleResources* resources = app->getResources();
	ModuleD3D12* d3d12 = app->getD3D12();
	ModuleShaderDescriptors* shaderDescriptors = app->getShaderDescriptors();

	D3D12_RESOURCE_DESC textureDesc = {};
	textureDesc.MipLevels = 1;
	textureDesc.Format = format;
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	textureDesc.DepthOrArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;
	texture = resources->createRenderTarget(width, height, format, clearColor, name);
	rtvHandle = shaderDescriptors->getCPUHandle(shaderDescriptors->allocteDescriptor());
	srvHandle = shaderDescriptors->getCPUHandle(shaderDescriptors->allocteDescriptor());
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = format;
	rtvDesc.Texture2D.MipSlice = 0;
	d3d12->getDevice()->CreateRenderTargetView(texture.Get(), &rtvDesc, rtvHandle);
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 1;
	d3d12->getDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, srvHandle);

	shaderDescriptors->createSRV(texture.Get(), 0);
}
