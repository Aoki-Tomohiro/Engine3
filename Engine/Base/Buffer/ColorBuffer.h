#pragma once
#include "GpuResource.h"
#include <cstdint>
#include <dxgi1_6.h>

class ColorBuffer : public GpuResource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ColorBuffer() override;

	/// <summary>
	/// 作成
	/// </summary>
	/// <param name="device"></param>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="format"></param>
	/// <param name="clearColor"></param>
	void Create(ID3D12Device* device, int32_t width, int32_t height, DXGI_FORMAT format, float* clearColor);

	/// <summary>
	/// スワップチェーンから作成
	/// </summary>
	/// <param name="swapChain"></param>
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
	/// SRVハンドルを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() const { return srvHandle_; };

	/// <summary>
	/// SRVハンドルを設定
	/// </summary>
	/// <param name="srvHandle"></param>
	void SetSRVHandle(const D3D12_CPU_DESCRIPTOR_HANDLE& srvHandle) { srvHandle_ = srvHandle; };

	/// <summary>
	/// クリアカラーを取得
	/// </summary>
	/// <returns></returns>
	const float* GetClearColor() const { return clearColor_; };

private:
	//RTVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	//SRVハンドル
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
	//クリアカラー
	float clearColor_[4];
};

