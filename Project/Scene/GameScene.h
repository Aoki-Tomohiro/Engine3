#pragma once
#include "Engine/Framework/Scene/IScene.h"
#include "Engine/Base/Renderer/Renderer.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Components/CollisionManager/CollisionManager.h"

#include "Project/GameObject/Character/Player.h"
#include "Project/GameObject/Character/Enemy.h"
#include "Project/GameObject/Skydome/Skydome.h"
#include "Project/GameObject/FollowCamera/FollowCamera.h"
#include "Project/GameObject/Floor/Floor.h"
#include "Project/GameObject/Goal/Goal.h"
#include "Project/GameObject/LockOn/LockOn.h"

class GameScene : public IScene {
public:
	//床の数
	static const int kFloorMax = 5;

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
	//衝突マネージャー
	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;
	//プレイヤー
	std::unique_ptr<Model> playerModel_ = nullptr;
	std::unique_ptr<Model> weaponModel_ = nullptr;
	std::unique_ptr<Player> player_ = nullptr;
	//敵
	std::unique_ptr<Model> modelEnemyBody_ = nullptr;
	std::unique_ptr<Model> modelEnemyL_arm_ = nullptr;
	std::unique_ptr<Model> modelEnemyR_arm_ = nullptr;
	std::list<std::unique_ptr<Enemy>> enemies_{};
	//天球
	std::unique_ptr<Model> skydomeModel_ = nullptr;
	std::unique_ptr<Skydome> skydome_ = nullptr;
	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;
	//床
	std::unique_ptr<Model> floorModel_ = nullptr;
	std::vector<std::unique_ptr<Floor>> floors_{};
	//ゴール
	std::unique_ptr<Model> goalModel_ = nullptr;
	std::unique_ptr<Goal> goal_ = nullptr;
	//ロックオン
	std::unique_ptr<LockOn> lockOn_ = nullptr;
};

