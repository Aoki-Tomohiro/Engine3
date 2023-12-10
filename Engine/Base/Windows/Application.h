#pragma once
#include <Windows.h>
#include <cstdint>
#include "Engine/Externals/imgui/imgui_impl_win32.h"
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

class Application {
public:
	//ウィンドウサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Application* GetInstance();

	/// <summary>
	/// ウィンドウプロシージャ
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/// <summary>
	/// ゲームウィンドウの作成
	/// </summary>
	/// <param name="title"></param>
	/// <param name="clientWidth"></param>
	/// <param name="clientHeight"></param>
	void CreateGameWindow(const wchar_t* title, int32_t clientWidth, int32_t clientHeight);

	/// <summary>
	/// ゲームウィンドウを閉じる
	/// </summary>
	void CloseGameWindow();

	/// <summary>
	/// メッセージの処理
	/// </summary>
	/// <returns></returns>
	bool ProcessMessage();

	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	/// <returns></returns>
	HWND GetHwnd() const { return hwnd_; };

	/// <summary>
	/// インスタンスハンドルの取得
	/// </summary>
	/// <returns></returns>
	HINSTANCE GetHInstance() { return wc_.hInstance; };

private:
	Application() = default;
	~Application() = default;
	Application(const Application&) = delete;
	const Application& operator=(const Application&) = delete;

private:
	//ウィンドウクラス
	WNDCLASS wc_{};

	//ウィンドウサイズ
	RECT wrc_{};

	//ウィンドウハンドル
	HWND hwnd_{};
};