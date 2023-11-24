#include "SceneManager.h"
#include <cassert>

//実体定義
SceneManager* SceneManager::instance_ = nullptr;

SceneManager* SceneManager::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new SceneManager();
	}
	return instance_;
}

void SceneManager::DeleteInstance() {
	delete instance_;
	instance_ = nullptr;
}

SceneManager::~SceneManager() {
	//シーンの削除
	delete currentScene_;
}

void SceneManager::Update() {
	//次のシーンがあるなら
	if (nextScene_) {
		//旧シーンの終了
		if (currentScene_) {
			delete currentScene_;
		}
		//シーン切り替え
		currentScene_ = nextScene_;
		nextScene_ = nullptr;
		//シーンマネージャーをセット
		currentScene_->SetSceneManager(this);
		//シーンの初期化
		currentScene_->Initialize();
	}

	//シーンの更新
	currentScene_->Update();
}

void SceneManager::Draw() {
	//シーンの描画
	currentScene_->Draw();
}

void SceneManager::DrawUI() {
	//スプライトの描画
	currentScene_->DrawUI();
}

void SceneManager::ChangeScene(const std::string& sceneName) {
	assert(sceneFactory_);
	assert(nextScene_ == nullptr);
	//次のシーンを作成
	nextScene_ = sceneFactory_->CreateScene(sceneName);
}