#include "GameScene.h"

GameScene::~GameScene() {

}

void GameScene::Initialize() {
	//Rendererのインスタンスを取得
	renderer_ = Renderer::GetInstance();
}

void GameScene::Update() {

}

void GameScene::Draw() {
	//モデル描画
	renderer_->PreDrawModels(Renderer::Transparent);

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