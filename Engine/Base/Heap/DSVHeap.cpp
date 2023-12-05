#include "DSVHeap.h"
#include "Engine/Base/Graphics/GraphicsCommon.h"

uint32_t DSVHeap::descriptorSizeDSV = 0;

void DSVHeap::Create(UINT numDescriptors) {
	//デバイスを取得
	device_ = GraphicsCommon::GetInstance()->GetDevice();

	//インクリメントサイズの初期化
	descriptorSizeDSV = device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	//ディスクリプタの数を初期化
	numDescriptors_ = numDescriptors;

	//ディスクリプタヒープの作成
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device_->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap_));
	assert(SUCCEEDED(hr));
}

void DSVHeap::CreateDepthStencilView(DepthBuffer& resource, DXGI_FORMAT format) {
	index_++;
	//ディスクリプタの最大数を超えていたら止める
	if (index_ > numDescriptors_) {
		assert(false);
	}

	//DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;//Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;//2DTexture

	//DSVHeapの先頭にDSVを作る
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCPUDescriptorHandle(index_);
	device_->CreateDepthStencilView(resource.GetResource(), &dsvDesc, dsvHandle);

	resource.SetDSVHandle(dsvHandle);
}

D3D12_CPU_DESCRIPTOR_HANDLE DSVHeap::GetCPUDescriptorHandle(uint32_t index) {
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += static_cast<D3D12_CPU_DESCRIPTOR_HANDLE>((descriptorSizeDSV * index)).ptr;
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE DSVHeap::GetGPUDescriptorHandle(uint32_t index) {
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap_->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += static_cast<D3D12_GPU_DESCRIPTOR_HANDLE>((descriptorSizeDSV * index)).ptr;
	return handleGPU;
}