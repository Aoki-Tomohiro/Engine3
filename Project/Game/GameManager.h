#pragma once
#include "Engine/Framework/Core/EngineCore.h"
#include "Project/Scene/SceneFactory.h"

class GameManager : public EngineCore {
public:
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

private:
};

