#include "GameScene.h"

GameScene::~GameScene() {

}

void GameScene::Initialize() {
	//Rendererのインスタンスを取得
	renderer_ = Renderer::GetInstance();

	//カメラの初期化
	camera_.Initialize();

	//衝突マネージャーの作成
	collisionManager_ = std::make_unique<CollisionManager>();

	//ロックオンの初期化
	lockOn_ = std::make_unique<LockOn>();
	lockOn_->Initialize();

	//プレイヤーの作成
	playerModel_.reset(Model::CreateFromOBJ("Resources/Models/Player", "Player.obj"));
	weaponModel_.reset(Model::CreateFromOBJ("Resources/Models/Weapon", "Weapon.obj"));
	std::vector<Model*> playerModels = { playerModel_.get(),weaponModel_.get() };
	player_ = std::make_unique<Player>();
	player_->Initialize(playerModels);
	player_->SetCamera(&camera_);
	player_->SetLockOn(lockOn_.get());

	//敵の作成
	modelEnemyBody_.reset(Model::CreateFromOBJ("Resources/Models/Enemy_Body", "Enemy_Body.obj"));
	modelEnemyL_arm_.reset(Model::CreateFromOBJ("Resources/Models/Enemy_L_arm", "Enemy_L_arm.obj"));
	modelEnemyR_arm_.reset(Model::CreateFromOBJ("Resources/Models/Enemy_R_arm", "Enemy_R_arm.obj"));
	std::vector<Model*> enemyModels = { modelEnemyBody_.get(),modelEnemyL_arm_.get(),modelEnemyR_arm_.get() };
	Enemy* enemy = new Enemy();
	enemy->Initialize(enemyModels);
	enemies_.push_back(std::unique_ptr<Enemy>(enemy));

	//天球の作成
	skydomeModel_.reset(Model::CreateFromOBJ("Resources/Models/Skydome", "Skydome.obj"));
	skydomeModel_->GetDirectionalLight()->SetEnableLighting(false);
	skydome_ = std::make_unique<Skydome>();
	skydome_->Initialize(skydomeModel_.get());

	//追従カメラの作成
	followCamera_ = std::make_unique<FollowCamera>();
	followCamera_->Initialize();
	followCamera_->SetTarget(&player_->GetWorldTransform());
	followCamera_->SetLockOn(lockOn_.get());

	//床の作成
	floorModel_.reset(Model::CreateFromOBJ("Resources/Models/Floor", "Floor.obj"));
	for (uint32_t i = 0; i < kFloorMax; ++i) {
		Floor* floor = new Floor();
		if (i % 2 == 1) {
			floor->Initialize(floorModel_.get(), { 0.0f,0.0f,i * 20.0f }, { i * 0.1f,0.0f,0.0f });
		}
		else {
			floor->Initialize(floorModel_.get(), { 0.0f,0.0f,i * 20.0f }, { 0.0f,0.0f,0.0f });
		}
		floors_.push_back(std::unique_ptr<Floor>(floor));
	}

	//ゴールの作成
	goalModel_.reset(Model::CreateFromOBJ("Resources/Models/Goal", "Goal.obj"));
	goal_ = std::make_unique<Goal>();
	goal_->Initialize(goalModel_.get(), { 0.0f,0.0f,4 * 20.0f });
}

void GameScene::Update() {
	//プレイヤーの更新
	player_->Update();

	//死亡した敵を削除
	enemies_.remove_if([](std::unique_ptr<Enemy>& enemy) {
		if (enemy->GetIsDeathAnimationEnd()) {
			enemy.reset();
			return true;
		}
		return false;
	});

	//敵の更新
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->SetIsPlayerAttack(player_->GetIsAttack());
		enemy->Update();
	}

	//天球の更新
	skydome_->Update();

	//床の更新
	for (std::unique_ptr<Floor>& floor : floors_) {
		floor->Update();
	}

	//ゴールの更新
	goal_->Update();

	//ロックオン更新
	lockOn_->Update(enemies_, camera_);

	//追従カメラの更新
	followCamera_->Update();
	camera_.translation_ = followCamera_->GetCamera().translation_;
	camera_.rotation_ = followCamera_->GetCamera().rotation_;
	camera_.quaternion_ = followCamera_->GetCamera().quaternion_;
	camera_.matView_ = followCamera_->GetCamera().matView_;
	camera_.matProjection_ = followCamera_->GetCamera().matProjection_;

	//カメラの更新
	camera_.TransferMatrix();

	//衝突判定
	collisionManager_->ClearColliderList();
	collisionManager_->SetColliderList(player_.get());
	if (player_->GetWeapon()->GetIsAttack()) {
		collisionManager_->SetColliderList(player_->GetWeapon());
	}
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		collisionManager_->SetColliderList(enemy.get());
	}
	for (std::unique_ptr<Floor>& floor : floors_) {
		collisionManager_->SetColliderList(floor.get());
	}
	collisionManager_->SetColliderList(goal_.get());
	collisionManager_->CheckAllCollisions();
}

void GameScene::Draw() {
	//モデル描画
	renderer_->PreDrawModels(Renderer::Transparent);

	//プレイヤーの描画
	player_->Draw(camera_);

	//天球の描画
	skydome_->Draw(camera_);

	//床の描画
	for (std::unique_ptr<Floor>& floor : floors_) {
		floor->Draw(camera_);
	}

	//ゴールの描画
	goal_->Draw(camera_);

	renderer_->PostDrawModels();

	//透明モデル描画
	renderer_->PreDrawModels(Renderer::Opaque);

	//敵の描画
	for (const std::unique_ptr<Enemy>& enemy : enemies_) {
		enemy->Draw(camera_);
	}

	renderer_->PostDrawModels();

	//パーティクル描画
	renderer_->PreDrawParticles();

	player_->DrawParticle(camera_);

	renderer_->PostDrawParticles();
}

void GameScene::DrawUI() {
	//スプライト描画
	renderer_->PreDrawSprites(Renderer::kBlendModeNormal);

	lockOn_->Draw();

	renderer_->PostDrawSprites();
}