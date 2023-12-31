#pragma once
#include "Heap.h"
#include "Engine/Base/Buffer/ColorBuffer.h"

class RTVHeap : public Heap {
public:
	//インクリメントサイズ
	static uint32_t descriptorSizeRTV;

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="numDescriptors"></param>
	void Create(UINT numDescriptors) override;

	/// <summary>
	/// RTVを作成
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="format"></param>
	void CreateRenderTargetView(ColorBuffer& resource, DXGI_FORMAT format);

private:
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

private:

};

