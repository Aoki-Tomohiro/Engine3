#include "GameManager.h"

void GameManager::Initialize() {
	//基底クラスの初期化
	EngineCore::Initialize();
	//シーンの生成
	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());
	sceneManager_->ChangeScene("GameScene");
}

void GameManager::Update() {
	//基底クラスの更新
	EngineCore::Update();
}

void GameManager::Draw() {
	//基底クラスの描画
	EngineCore::Draw();
}