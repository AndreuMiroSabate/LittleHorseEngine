#include "Globals.h"
#include "ModuleSamplers.h"

void ModuleSamplers::init(ID3D12Device* device) {
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.NumDescriptors = MAX_SAMPLERS;
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
	heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&samplerHeap));
	samplerDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
	CreateDefaultSamplers(device);
}

void ModuleSamplers::CreateDefaultSamplers(ID3D12Device* device) {
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = samplerHeap->GetCPUDescriptorHandleForHeapStart();
	D3D12_SAMPLER_DESC samplers[2] = {
		// 1. Linear Wrap Sampler
		{
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, 
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, 
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,
			0.0f, 
			1, 
			D3D12_COMPARISON_FUNC_ALWAYS, 
			{ 0, 0, 0, 0 }, 
			0.0f, D3D12_FLOAT32_MAX 
		},
		// 2. Point Clamp Sampler
		{
			D3D12_FILTER_MIN_MAG_MIP_POINT,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
			0.0f,
			1,
			D3D12_COMPARISON_FUNC_ALWAYS,
			{ 0, 0, 0, 0 },
			0.0f, D3D12_FLOAT32_MAX
		},
	};
	for (int i = 0; i < std::size(samplers); i++) {
		device->CreateSampler(&samplers[i], cpuHandle);
		cpuHandle.ptr += samplerDescriptorSize;
	}
}
