#pragma once
#include "Heap.h"
#include "Engine/Base/Buffer/TextureResource.h"
#include "Engine/Base/Buffer/ColorBuffer.h"
#include "Engine/Base/Buffer/UploadBuffer.h"

class SRVHeap : public Heap {
public:
	//インクリメントサイズ
	static uint32_t descriptorSizeSRV;

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="numDescriptors"></param>
	void Create(UINT numDescriptors) override;

	/// <summary>
	/// SRVの作成
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="format"></param>
	void CreateShaderResourceView(ColorBuffer& resource, DXGI_FORMAT format);

	/// <summary>
	/// SRVの作成
	/// </summary>
	/// <param name="resource"></param>
	/// <param name="format"></param>
	/// <param name="mipLevels"></param>
	void CreateShaderResourceView(TextureResource& resource, DXGI_FORMAT format, UINT mipLevels);

	/// <summary>
	/// Instancing用のSRVの作成
	/// </summary>
	/// <param name="instancingResource"></param>
	/// <param name="kNumInstance"></param>
	/// <param name="size"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE CreateInstancingShaderResourceView(UploadBuffer& instancingResource, uint32_t kNumInstance, size_t size);

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

