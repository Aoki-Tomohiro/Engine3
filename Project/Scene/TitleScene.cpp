#include "TitleScene.h"

TitleScene::~TitleScene() {

}

void TitleScene::Initialize() {
	//Rendererのインスタンスを取得
	renderer_ = Renderer::GetInstance();
}

void TitleScene::Update() {

}

void TitleScene::Draw() {
	//モデル描画
	renderer_->PreDrawModels(Renderer::Transparent);

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