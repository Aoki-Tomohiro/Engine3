#pragma once
#include "IScene.h"
#include "AbstractSceneFactory.h"

class SceneManager {
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static SceneManager* GetInstance();

	/// <summary>
	/// シングルトンインスタンスの削除
	/// </summary>
	static void DeleteInstance();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// スプライト描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 次のシーン予約
	/// </summary>
	/// <param name="sceneName"></param>
	void ChangeScene(const std::string& sceneName);

	/// <summary>
	/// シーンファクトリーをセット
	/// </summary>
	/// <param name="sceneFactory"></param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; };

private:
	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	const SceneManager& operator=(const SceneManager&) = delete;

private:
	//インスタンス
	static SceneManager* instance_;

	//現在のシーン
	IScene* currentScene_ = nullptr;

	//次のシーン
	IScene* nextScene_ = nullptr;

	//シーンファクトリー
	AbstractSceneFactory* sceneFactory_ = nullptr;
};

