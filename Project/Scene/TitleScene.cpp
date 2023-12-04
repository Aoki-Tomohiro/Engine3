#include "TitleScene.h"

TitleScene::~TitleScene() {

}

void TitleScene::Initialize() {
	//Rendererのインスタンスを取得
	renderer_ = Renderer::GetInstance();
	//カメラの初期化
	camera_.Initialize();
	//モデルの生成
	model_.reset(Model::CreateFromOBJ("Resources/Models/Cube", "Cube.obj"));
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void TitleScene::Update() {
	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
	//カメラの更新
	camera_.UpdateMatrix();
}

void TitleScene::Draw() {
	//モデル描画
	renderer_->PreDrawModels(Renderer::Transparent);

	//モデルの描画
	model_->Draw(worldTransform_, camera_);

	renderer_->PostDrawModels();

	//パーティクル描画
	renderer_->PreDrawParticles();

	renderer_->PostDrawParticles();
}

void TitleScene::DrawUI() {
	//スプライト描画
	renderer_->PreDrawSprites(Renderer::kBlendModeNormal);

	renderer_->PostDrawSprites();
}