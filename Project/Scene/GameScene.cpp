#include "GameScene.h"

GameScene::~GameScene() {

}

void GameScene::Initialize() {
	//Rendererのインスタンスを取得
	renderer_ = Renderer::GetInstance();
	//カメラの初期化
	camera_.Initialize();
	//モデルの生成
	model_.reset(Model::CreateFromOBJ("Resources/Models/Cube", "Cube.obj"));
	//InputHandlerの生成
	inputHandler_ = std::make_unique<InputHandler>();
	//コマンドを割り当てる
	inputHandler_->AssingMoveLeftCommand2PressKeyA();
	inputHandler_->AssingMoveRightCommand2PressKeyD();
	//プレイヤーの生成
	player_ = std::make_unique<Player>();
	player_->Initialize(model_.get());
}

void GameScene::Update() {
	//キーを取得
	command_ = inputHandler_->HandleInput();

	//コマンドを実行
	if (this->command_) {
		command_->Exec(*player_);
	}

	//プレイヤーの更新
	player_->Update();

	//カメラの更新
	camera_.UpdateMatrix();

	ImGui::Begin("GameScene");
	ImGui::Text("AD : Move");
	ImGui::End();
}

void GameScene::Draw() {
	//モデル描画
	renderer_->PreDrawModels(Renderer::Transparent);

	//プレイヤーの描画
	player_->Draw(camera_);

	renderer_->PostDrawModels();

	//パーティクル描画
	renderer_->PreDrawParticles();

	renderer_->PostDrawParticles();
}

void GameScene::DrawUI() {
	//スプライト描画
	renderer_->PreDrawSprites(Renderer::kBlendModeNormal);

	renderer_->PostDrawSprites();
}