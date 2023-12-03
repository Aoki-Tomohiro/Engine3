#pragma once
#include "Engine/Base/GraphicsCore/GraphicsCore.h"

class Heap {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Heap() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="numDescriptors"></param>
	virtual void Initialize(UINT numDescriptors) = 0;

	/// <summary>
	/// CPUディスクリプタハンドルを取得
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	virtual D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index) = 0;

	/// <summary>
	/// GPUディスクリプタハンドルを取得
	/// </summary>
	/// <param name="descriptorHeap"></param>
	/// <param name="descriptorSize"></param>
	/// <param name="index"></param>
	/// <returns></returns>
	virtual D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index) = 0;

	/// <summary>
	/// ディスクリプタヒープを取得
	/// </summary>
	/// <returns></returns>
	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); };

protected:
	//デバイス
	ID3D12Device* device_ = nullptr;
	//ディスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;
	//ディスクリプタの最大数
	uint32_t numDescriptors_ = 0;
	//インデックス
	uint32_t index_ = -1;
};
