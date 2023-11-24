#include "TitleScene.h"
#include "Engine/Math/MathFunction.h"

TitleScene::~TitleScene() {

}

void TitleScene::Initialize() {
	//Rendererのインスタンスを取得
	renderer_ = Renderer::GetInstance();
}

void TitleScene::Update() {
	Quaternion rotation0 = MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	Quaternion rotation1 = { -rotation0.x,-rotation0.y,-rotation0.z,-rotation0.w };

	Quaternion interpolate0 = Slerp(rotation0, rotation1, 0.0f);
	Quaternion interpolate1 = Slerp(rotation0, rotation1, 0.3f);
	Quaternion interpolate2 = Slerp(rotation0, rotation1, 0.5f);
	Quaternion interpolate3 = Slerp(rotation0, rotation1, 0.7f);
	Quaternion interpolate4 = Slerp(rotation0, rotation1, 1.0f);

	ImGui::Begin("TitleScene");
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate0, Slerp(q0,q1,0.0f)",
		interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate1, Slerp(q0,q1,0.3f)",
		interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate2, Slerp(q0,q1,0.5f)",
		interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate3, Slerp(q0,q1,0.7f)",
		interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate4, Slerp(q0,q1,1.0f)",
		interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);
	ImGui::End();
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