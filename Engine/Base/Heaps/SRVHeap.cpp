#include "SRVHeap.h"

void SRVHeap::Initialize(UINT numDescriptors) {
	//デバイスを取得
	device_ = GraphicsCore::GetInstance()->GetDevice();

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

D3D12_CPU_DESCRIPTOR_HANDLE SRVHeap::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += static_cast<D3D12_CPU_DESCRIPTOR_HANDLE>((GraphicsCore::descriptorSizeSRV * index)).ptr;
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SRVHeap::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>((GraphicsCore::descriptorSizeSRV * index)).ptr;
	return handleGPU;
}

uint32_t SRVHeap::CreateShaderResourceView(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format) {
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
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = GetCPUDescriptorHandle(index_);
	device_->CreateShaderResourceView(resource.Get(), &srvDesc, srvHandle);

	return index_;
}

uint32_t SRVHeap::CreateShaderResourceView(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format, UINT mipLevels) {
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
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = GetCPUDescriptorHandle(index_);
	device_->CreateShaderResourceView(resource.Get(), &srvDesc, srvHandle);

	return index_;
}

uint32_t SRVHeap::CreateInstancingShaderResourceView(const Microsoft::WRL::ComPtr<ID3D12Resource>& instancingResource, uint32_t kNumInstance, size_t size) {
	index_++;
	//ディスクリプタの最大数を超えていたら止める
	if (index_ < numDescriptors_) {
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

	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = GetCPUDescriptorHandle(index_);
	device_->CreateShaderResourceView(instancingResource.Get(), &instancingSrvDesc, srvHandle);

	return index_;
}