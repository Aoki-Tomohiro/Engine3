#pragma once
#include "GpuResource.h"

class UploadBuffer : public GpuResource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~UploadBuffer() override;

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="sizeInBytes"></param>
	void Create(size_t sizeInBytes);

	/// <summary>
	/// マップ
	/// </summary>
	/// <returns></returns>
	void* Map();

	/// <summary>
	/// アンマップ
	/// </summary>
	void Unmap();

	/// <summary>
	/// サイズを取得
	/// </summary>
	/// <returns></returns>
	size_t GetBufferSize() const { return bufferSize_; }

	/// <summary>
	/// GPUアドレスを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const { return resource_->GetGPUVirtualAddress(); };

private:
	//サイズ
	size_t bufferSize_ = 0;
	//マップされたデータ
	void* mappedData_ = nullptr;
};

