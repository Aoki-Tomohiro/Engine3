#include "RTVHeap.h"

uint32_t RTVHeap::descriptorSizeRTV = 0;

void RTVHeap::Create(UINT numDescriptors) {
	//デバイスを取得
	device_ = GraphicsDevice::GetInstance()->GetDevice();

	//インクリメントサイズの初期化
	descriptorSizeRTV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	//ディスクリプタの数を初期化
	numDescriptors_ = numDescriptors;

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(hr));
}

void RTVHeap::CreateRenderTargetView(ColorBuffer& resource, DXGI_FORMAT format) {
	index_++;
	//ディスクリプタの最大数を超えていたら止める
	if (index_ > numDescriptors_) {
		assert(false);
	}

	//RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Format = format;

	//RTVの作成
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCPUDescriptorHandle(index_);
	device_->CreateRenderTargetView(resource.GetResource(), &rtvDesc, rtvHandle);

	resource.SetRTVHandle(rtvHandle);
}

D3D12_CPU_DESCRIPTOR_HANDLE RTVHeap::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += static_cast<D3D12_CPU_DESCRIPTOR_HANDLE>((descriptorSizeRTV * index)).ptr;
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE RTVHeap::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>((descriptorSizeRTV * index)).ptr;
	return handleGPU;
}