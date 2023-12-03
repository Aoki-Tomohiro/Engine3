#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer/Renderer.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/Model.h"
#include "Project/GameObject/InputHandler.h"
#include "Project/GameObject/Player.h"

class GameScene : public IScene {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene() override;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw() override;

	/// <summary>
	/// スプライト描画
	/// </summary>
	void DrawUI() override;

private:
	//Renderer
	Renderer* renderer_ = nullptr;
	//カメラ
	Camera camera_{};
	//モデル
	std::unique_ptr<Model> model_ = nullptr;
	//InputHandler
	std::unique_ptr<InputHandler> inputHandler_ = nullptr;
	//Command
	ICommand* command_ = nullptr;
	//Player
	std::unique_ptr<Player> player_ = nullptr;
};

