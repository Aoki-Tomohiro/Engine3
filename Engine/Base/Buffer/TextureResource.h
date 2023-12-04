#pragma once
#include "GpuResource.h"
#include "Engine/Externals/DirectXTex/DirectXTex.h"
#include "Engine/Externals/DirectXTex/d3dx12.h"

class TextureResource : public GpuResource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~TextureResource() override = default;

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="metadata"></param>
	void Create(const DirectX::TexMetadata& metadata);

	/// <summary>
	/// Cpuハンドルを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVCpuHandle() const { return cpuHandle_; };

	/// <summary>
	/// Cpuハンドルを設定
	/// </summary>
	/// <param name="cpuHandle"></param>
	void SetSRVCpuHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& cpuHandle) { cpuHandle_ = cpuHandle; };

	/// <summary>
	/// Gpuハンドルを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVGpuHandle() const { return gpuHandle_; };

	/// <summary>
	/// Gpuハンドルを設定
	/// </summary>
	/// <param name="cpuHandle"></param>
	void SetSRVGpuHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& gpuHandle) { gpuHandle_ = gpuHandle; };

	/// <summary>
	/// リソースの設定を取得
	/// </summary>
	/// <returns></returns>
	const D3D12_RESOURCE_DESC& GetResourceDesc() const { return resourceDesc_; };

private:
	//リソースの設定
	D3D12_RESOURCE_DESC resourceDesc_{};
	//CpuHandle
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};
	//GpuHandle
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};
};

