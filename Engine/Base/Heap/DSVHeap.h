#pragma once
#include "Heap.h"

class DSVHeap : public Heap {
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
	/// DSVの作成
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="format"></param>
	/// <returns></returns>
	uint32_t CreateDepthStencilView(const Microsoft::WRL::ComPtr<ID3D12Resource>& resource, DXGI_FORMAT format);
	
private:

};

