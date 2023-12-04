#pragma once
#include "Engine/Base/Windows/Application.h"
#include "Engine/Base/Graphics/GraphicsDevice.h"
#include "Engine/Base/Heap/SRVHeap.h"
#include "Engine/Externals/imgui/imgui.h"
#include "Engine/Externals/imgui/imgui_impl_dx12.h"
#include <memory>

class ImGuiManager {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static ImGuiManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ImGui受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// </summary>
	void End();

	/// <summary>
	/// ImGuiの描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ImGuiの解放処理
	/// </summary>
	void ShutDown();

private:
	ImGuiManager() = default;
	~ImGuiManager() = default;
	ImGuiManager(const ImGuiManager&) = delete;
	ImGuiManager& operator=(const ImGuiManager&) = delete;

private:
	//ウィンドウズアプリケーション
	Application* app_ = nullptr;
	//DirectXCommon
	GraphicsDevice* device_ = nullptr;
	//SRV用ヒープ
	std::unique_ptr<SRVHeap> srvDescriptorHeap_ = nullptr;
};

