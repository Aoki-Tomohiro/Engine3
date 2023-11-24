#include "SceneFactory.h"

SceneFactory::~SceneFactory() {

}

IScene* SceneFactory::CreateScene(const std::string& sceneName) {
	//次のシーンを生成
	IScene* newScene = nullptr;

	if (sceneName == "TitleScene") {
		newScene = new TitleScene();
	}
	else if (sceneName == "GameScene") {
		newScene = new GameScene();
	}

	return newScene;
}