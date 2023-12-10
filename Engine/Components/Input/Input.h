#pragma once
#include "Engine/Base/Windows/Application.h"
#include <memory>
#include <wrl.h>
#define DIRECTINPUT_VERSION 0x0800//DirectInputのバージョン指定
#include <dinput.h>
#include <Xinput.h>
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"xinput.lib")

class Input {
private:
	//エイリアステンプレート
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public:
	/// <summary>
	/// シングルインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Input* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// キーが押されているかを判定
	/// </summary>
	/// <param name="keyNum"></param>
	/// <returns></returns>
	bool IsPushKey(uint8_t keyNum);

	/// <summary>
	/// キーを押した瞬間を判定
	/// </summary>
	/// <param name="keyNum"></param>
	/// <returns></returns>
	bool IsPushKeyEnter(uint8_t keyNum);

	/// <summary>
	/// キーを離した瞬間を判定
	/// </summary>
	/// <param name="keyNum"></param>
	/// <returns></returns>
	bool IsPushKeyExit(uint8_t keyNum);

	/// <summary>
	/// マウスが押されているかを判定
	/// </summary>
	/// <param name="mouseNum"></param>
	/// <returns></returns>
	bool IsPressMouse(int32_t mouseNum);

	/// <summary>
	/// マウスが押された瞬間を判定
	/// </summary>
	/// <param name="mouseNum"></param>
	/// <returns></returns>
	bool IsPressMouseEnter(int32_t mouseNum);

	/// <summary>
	/// マウスが離された瞬間を判定
	/// </summary>
	/// <param name="mouseNum"></param>
	/// <returns></returns>
	bool IsPressMouseExit(int32_t mouseNum);

	/// <summary>
	/// ホイールスクロール量を取得する
	/// </summary>
	/// <returns></returns>
	int32_t GetWheel();

	/// <summary>
	/// コントローラーが接続されているかを判定
	/// </summary>
	/// <returns></returns>
	bool IsControllerConnected();

	/// <summary>
	/// ボタンが押されているかを判定
	/// </summary>
	/// <param name="button"></param>
	/// <returns></returns>
	bool IsPressButton(WORD button);

	/// <summary>
	/// ボタンが押された瞬間を判定
	/// </summary>
	/// <param name="button"></param>
	/// <returns></returns>
	bool IsPressButtonEnter(WORD button);

	/// <summary>
	/// ボタンが離された瞬間を判定
	/// </summary>
	/// <param name="button"></param>
	/// <returns></returns>
	bool IsPressButtonExit(WORD button);

	/// <summary>
	/// 左スティックのX軸の値を取得
	/// </summary>
	/// <returns></returns>
	float GetLeftStickX();

	/// <summary>
	/// 左スティックのY軸の値を取得
	/// </summary>
	/// <returns></returns>
	float GetLeftStickY();

	/// <summary>
	/// 右スティックのX軸の値を取得
	/// </summary>
	/// <returns></returns>
	float GetRightStickX();

	/// <summary>
	/// 右スティックのY軸の値を取得
	/// </summary>
	/// <returns></returns>
	float GetRightStickY();

private:
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	const Input& operator = (const Input&) = delete;

private:
	//WindowsAPI
	Application* app_ = nullptr;

	//DirectInput
	ComPtr<IDirectInput8> directInput_ = nullptr;

	//KeyboardDevice
	ComPtr<IDirectInputDevice8> keyboardDevice_ = nullptr;

	//MouseDevice
	ComPtr<IDirectInputDevice8> mouseDevice_ = nullptr;

	//キー情報
	BYTE key_[256] = {};

	//前フレームのキー情報
	BYTE preKey_[256] = {};

	//マウスの情報
	DIMOUSESTATE mouse_ = {};

	//前のフレームのマウスの情報
	DIMOUSESTATE mousePre_ = {};

	//コントローラーの情報
	XINPUT_STATE state_{};

	//前のフレームのコントローラーの情報
	XINPUT_STATE preState_{};
};