#pragma once
#include "Engine/Framework/Scene/AbstractSceneFactory.h"
#include "Project/Scene/TitleScene.h"
#include "Project/Scene/GameScene.h"

class SceneFactory : public AbstractSceneFactory {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneFactory() override;

	/// <summary>
	/// シーンの作成
	/// </summary>
	/// <param name="sceneName"></param>
	/// <returns></returns>
	IScene* CreateScene(const std::string& sceneName) override;
};

