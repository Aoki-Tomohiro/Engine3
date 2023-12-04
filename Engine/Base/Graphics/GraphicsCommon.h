#pragma once
#include "GraphicsDevice.h"
#include "GraphicsContext.h"
#include "Engine/Base/Windows/Application.h"
#include "Engine/Base/Heap/RTVHeap.h"
#include "Engine/Base/Heap/DSVHeap.h"
#include <chrono>
#include <memory>

class GraphicsCommon {
public:
	/// <summary>
	/// シングルトンインスタンスを取得
	/// </summary>
	/// <returns></returns>
	static GraphicsCommon* GetInstance();

	/// <summary>
	/// シングルトンインスタンスを削除
	/// </summary>
	static void DeleteInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

private:
	GraphicsCommon() = default;
	~GraphicsCommon() = default;
	GraphicsCommon(const GraphicsCommon&) = delete;
	GraphicsCommon& operator=(const GraphicsCommon&) = delete;

	/// <summary>
	/// スワップチェーンの作成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// ディスクリプタヒープの作成
	/// </summary>
	void CreateDescriptorHeaps();

	/// <summary>
	/// スワップチェーンのリソースを作成
	/// </summary>
	void CreateSwapChainResources();

	/// <summary>
	/// RTVの作成
	/// </summary>
	void CreateRenderTargetViews();

	/// <summary>
	/// DepthBufferとDSVの作成
	/// </summary>
	void CreateDepthBufferAndView();

	/// <summary>
	/// FPS固定初期化
	/// </summary>
	void InitializeFixFPS();

	/// <summary>
	/// FPS固定更新
	/// </summary>
	void UpdateFixFPS();

private:
	//インスタンス
	static GraphicsCommon* instance_;
	//Application
	Application* app_ = nullptr;
	//GraphicsDevice
	GraphicsDevice* graphicsDevice_ = nullptr;
	//GraphicsContext
	GraphicsContext* graphicsContext_ = nullptr;
	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_ = nullptr;
	//ディスクリプタヒープ
	std::unique_ptr<RTVHeap> rtvDescriptorHeap_ = nullptr;
	std::unique_ptr<DSVHeap> dsvDescriptorHeap_ = nullptr;
	//リソース
	std::unique_ptr<ColorBuffer> swapChainResource_[2] = { nullptr };
	std::unique_ptr<DepthBuffer> depthStencilResource_ = nullptr;
	//記録時間(FPS固定用)
	std::chrono::steady_clock::time_point reference_{};
};

