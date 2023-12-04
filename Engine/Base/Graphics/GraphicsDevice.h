#pragma once
#include "Engine/Utilities/Log.h"
#include <cassert>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")

class GraphicsDevice {
public:
	/// <summary>
	/// シングルトンインスタンスを取得
	/// </summary>
	/// <returns></returns>
	static GraphicsDevice* GetInstance();

	/// <summary>
	/// シングルトンインスタンスを削除
	/// </summary>
	static void DeleteInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// デバイスを取得
	/// </summary>
	/// <returns></returns>
	ID3D12Device* GetDevice() const { return device_.Get(); };

	/// <summary>
	/// DXGIFactoryを取得
	/// </summary>
	/// <returns></returns>
	IDXGIFactory7* GetDXGIFactory() const { return dxgiFactory_.Get(); };

private:
	GraphicsDevice() = default;
	~GraphicsDevice() = default;
	GraphicsDevice(const GraphicsDevice&) = delete;
	GraphicsDevice& operator=(const GraphicsDevice&) = delete;

private:
	//インスタンス
	static GraphicsDevice* instance_;
	//DXGIファクトリー
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_ = nullptr;
	//アダプタ
	Microsoft::WRL::ComPtr<IDXGIAdapter4> useAdapter_ = nullptr;
	//デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device_ = nullptr;
};

