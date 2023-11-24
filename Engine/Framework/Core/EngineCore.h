#pragma once
#include "Engine/Base/Windows/Application.h"
#include "Engine/Base/GraphicsCore/GraphicsCore.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include "Engine/Base/Renderer/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Framework/Scene/AbstractSceneFactory.h"
#include "Engine/Utilities/D3DResourceLeakChecker.h"
#include <memory>

class EngineCore {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~EngineCore();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// 終了リクエスト
	/// </summary>
	/// <returns></returns>
	virtual bool IsEndRequest();

	/// <summary>
	/// ゲームループ
	/// </summary>
	void Run();

protected:
	//リークチェッカー
	D3DResourceLeakChecker leakChecker_{};
	//WindowsApplication
	Application* app_ = nullptr;
	//GraphicsCore
	GraphicsCore* core_ = nullptr;
	//テクスチャマネージャー
	TextureManager* textureManager_ = nullptr;
	//ImGuiManager
	ImGuiManager* imguiManager_ = nullptr;
	//Renderer
	Renderer* renderer_ = nullptr;
	//Input
	Input* input_ = nullptr;
	//Audio
	Audio* audio_ = nullptr;
	//シーンマネージャー
	SceneManager* sceneManager_ = nullptr;
	//シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
};

