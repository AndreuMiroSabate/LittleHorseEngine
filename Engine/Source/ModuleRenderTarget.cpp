#include "Globals.h"
#include "ModuleRenderTarget.h"

#include "Application.h"
#include "ModuleD3D12.h"
#include "ModuleShaderDescriptors.h"

ModuleRenderTarget::ModuleRenderTarget()
{
}
ModuleRenderTarget::~ModuleRenderTarget()
{
}

bool ModuleRenderTarget::init()
{
	device = app->getD3D12()->getDevice();
	return true;
}

void ModuleRenderTarget::preRender()
{
	if (needsResize)
	{
		createRenderTarget(width, height, format);
		//createDepthStencil(width, height);
		needsResize = false;
	}
}

void ModuleRenderTarget::createRenderTarget(UINT width, UINT height, DXGI_FORMAT format)
{
	this->width = width;
	this->height = height;
	this->format = format;

	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	D3D12_CLEAR_VALUE clearValue = {};
	clearValue.Format = format;
	clearValue.Color[0] = 0.2f;
	clearValue.Color[1] = 0.2f;
	clearValue.Color[2] = 0.2f;
	clearValue.Color[3] = 1.0f;

	CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

	device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &textureDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&texture));

	device->CreateRenderTargetView(texture.Get(), nullptr, rtvHandle);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	device->CreateShaderResourceView(texture.Get(), &srvDesc, srvHandle);
}
