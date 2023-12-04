#pragma once
#include "GpuResource.h"

class DepthBuffer : public GpuResource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~DepthBuffer() override = default;

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="format"></param>
	void Create(int32_t width, int32_t height, DXGI_FORMAT format);

	/// <summary>
	/// DSVハンドルを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetDSVHandle() const { return dsvHandle_; };

	/// <summary>
	/// DSVハンドルを設定
	/// </summary>
	/// <param name="dsvHandle"></param>
	void SetDSVHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& dsvHandle) { dsvHandle_ = dsvHandle; };

private:
	//DSVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};
};

