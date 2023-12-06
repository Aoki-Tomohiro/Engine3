#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer/Renderer.h"

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
};

