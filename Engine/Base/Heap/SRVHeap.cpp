#include "SRVHeap.h"
#include "Engine/Base/Graphics/GraphicsCommon.h"

uint32_t SRVHeap::descriptorSizeSRV = 0;

void SRVHeap::Create(UINT numDescriptors) {
	//デバイスを取得
	device_ = GraphicsCommon::GetInstance()->GetDevice();

	//インクリメントサイズの初期化
	descriptorSizeSRV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	//ディスクリプタの数を初期化
	numDescriptors_ = numDescriptors;

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(hr));
}

void SRVHeap::CreateShaderResourceView(ColorBuffer& resource, DXGI_FORMAT format) {
	index_++;
	//ディスクリプタの最大数を超えていたら止める
	if (index_ > numDescriptors_) {
		assert(false);
	}

	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//SRVの作成
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = GetCPUDescriptorHandle(index_);
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = GetGPUDescriptorHandle(index_);
	device_->CreateShaderResourceView(resource.GetResource(), &srvDesc, srvCpuHandle);

	resource.SetSRVCpuHandle(srvCpuHandle);
	resource.SetSRVGpuHandle(srvGpuHandle);
}

void SRVHeap::CreateShaderResourceView(TextureResource& resource, DXGI_FORMAT format, UINT mipLevels) {
	index_++;
	//ディスクリプタの最大数を超えていたら止める
	if (index_ > numDescriptors_) {
		assert(false);
	}

	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = format;
	srvDesc.Texture2D.MipLevels = mipLevels;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	//SRVの作成
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle = GetCPUDescriptorHandle(index_);
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = GetGPUDescriptorHandle(index_);
	device_->CreateShaderResourceView(resource.GetResource(), &srvDesc, srvCpuHandle);

	resource.SetSRVCpuHandle(srvCpuHandle);
	resource.SetSRVGpuHandle(srvGpuHandle);
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVHeap::CreateInstancingShaderResourceView(UploadBuffer& instancingResource, uint32_t kNumInstance, size_t size) {
	index_++;
	//ディスクリプタの最大数を超えていたら止める
	if (index_ > numDescriptors_) {
		assert(false);
	}

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumInstance;
	instancingSrvDesc.Buffer.StructureByteStride = UINT(size);

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCpu = GetCPUDescriptorHandle(index_);
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGpu = GetGPUDescriptorHandle(index_);
	device_->CreateShaderResourceView(instancingResource.GetResource(), &instancingSrvDesc, srvHandleCpu);

	return srvHandleGpu;
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVHeap::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += static_cast<D3D12_CPU_DESCRIPTOR_HANDLE>((descriptorSizeSRV * index)).ptr;
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVHeap::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>((descriptorSizeSRV * index)).ptr;
	return handleGPU;
}