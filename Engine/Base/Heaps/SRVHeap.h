#pragma once
#include "DescriptorHeap.h"

class SRVHeap : public DescriptorHeap {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="numDescriptors"></param>
	void Initialize(UINT numDescriptors) override;

	/// <summary>
	/// CPUディスクリプタハンドルを取得
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) override;

	/// <summary>
	/// GPUディスクリプタハンドルを取得
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) override;

	/// <summary>
	/// SRVの作成
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="format"></param>
	/// <returns></returns>
	uint32_t CreateShaderResourceView(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format);

	/// <summary>
	/// SRVの作成
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="format"></param>
	/// <param name="mipLevels"></param>
	/// <returns></returns>
	uint32_t CreateShaderResourceView(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format, UINT mipLevels);

	/// <summary>
	/// Instancing用のSRVの作成
	/// </summary>
	/// <param name="instancingResource"></param>
	/// <param name="kNumInstance"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	uint32_t CreateInstancingShaderResourceView(const Microsoft::WRL::ComPtr<ID3D12Resource>& instancingResource, uint32_t kNumInstance, size_t size);

private:

};

