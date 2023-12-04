#include "ColorBuffer.h"

void ColorBuffer::Create(int32_t width, int32_t height, DXGI_FORMAT format, float* clearColor) {
	//デバイスを取得
	device_ = GraphicsDevice::GetInstance()->GetDevice();

	//クリアカラーの初期化
	clearColor_[0] = clearColor[0];
	clearColor_[1] = clearColor[1];
	clearColor_[2] = clearColor[2];
	clearColor_[3] = clearColor[3];

	//リソースの状態の初期化
	resourceState_ = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;

	//ヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resourceDesc.Width = width;
	resourceDesc.Height = height;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

	//ClearValue
	D3D12_CLEAR_VALUE clearValue;
	clearValue.Format = format;
	clearValue.Color[0] = clearColor_[0];
	clearValue.Color[1] = clearColor_[1];
	clearValue.Color[2] = clearColor_[2];
	clearValue.Color[3] = clearColor_[3];

	//リソースの作成
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		resourceState_,
		&clearValue,
		IID_PPV_ARGS(&resource_));
	assert(SUCCEEDED(hr));
}

void ColorBuffer::CreateFromSwapChain(IDXGISwapChain* swapChain, UINT buffer, float* clearColor) {
	//クリアカラーの初期化
	clearColor_[0] = clearColor[0];
	clearColor_[1] = clearColor[1];
	clearColor_[2] = clearColor[2];
	clearColor_[3] = clearColor[3];

	//リソースの状態の初期化
	resourceState_ = D3D12_RESOURCE_STATE_PRESENT;

	//SwapChainからResourceを引っ張ってくる
	HRESULT hr = swapChain->GetBuffer(buffer, IID_PPV_ARGS(&resource_));
	//うまく取得できなければ起動できない
	assert(SUCCEEDED(hr));
}