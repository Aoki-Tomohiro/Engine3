#pragma once
#include "GpuResource.h"

class ColorBuffer : public GpuResource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ColorBuffer() override = default;

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="format"></param>
	/// <param name="clearColor"></param>
	void Create(int32_t width, int32_t height, DXGI_FORMAT format, float* clearColor);

	/// <summary>
	/// スワップチェーンから作成
	/// </summary>
	/// <param name="swapChain"></param>
	/// <param name="buffer"></param>
	/// <param name="clearColor"></param>
	void CreateFromSwapChain(IDXGISwapChain* swapChain, UINT buffer, float* clearColor);

	/// <summary>
	/// RTVハンドルを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle() const { return rtvHandle_; };

	/// <summary>
	/// RTVハンドルを設定
	/// </summary>
	/// <param name="rtvHandle"></param>
	void SetRTVHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& rtvHandle) { rtvHandle_ = rtvHandle; };

	/// <summary>
	/// SrvCpuハンドルを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVCpuHandle() const { return srvCpuHandle_; };

	/// <summary>
	/// SrvCpuハンドルを設定
	/// </summary>
	/// <param name="srvHandle"></param>
	void SetSRVCpuHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& srvCpuHandle) { srvCpuHandle_ = srvCpuHandle; };

	/// <summary>
	/// SrvGpuハンドルを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_GPU_DESCRIPTOR_HANDLE& GetSRVGpuHandle() const { return srvGpuHandle_; };

	/// <summary>
	/// SrvGpuハンドルを設定
	/// </summary>
	/// <param name="srvHandle"></param>
	void SetSRVGpuHandle(const D3D12_GPU_DESCRIPTOR_HANDLE& srvGpuHandle) { srvGpuHandle_ = srvGpuHandle; };

	/// <summary>
	/// クリアカラーを取得
	/// </summary>
	/// <returns></returns>
	const float* GetClearColor() const { return clearColor_; };

private:
	//RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	//SRVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvCpuHandle_;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle_;
	//クリアカラー
	float clearColor_[4];
};

