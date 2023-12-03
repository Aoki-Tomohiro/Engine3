#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer/Renderer.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"

class TitleScene : public IScene {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~TitleScene() override;

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
};

