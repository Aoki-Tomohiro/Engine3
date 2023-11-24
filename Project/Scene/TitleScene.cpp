#include "TitleScene.h"
#include "Engine/Math/MathFunction.h"

TitleScene::~TitleScene() {

}

void TitleScene::Initialize() {
	//Rendererのインスタンスを取得
	renderer_ = Renderer::GetInstance();
}

void TitleScene::Update() {
	Quaternion q1 = { 2.0f,3.0f,4.0f,1.0f };
	Quaternion q2 = { 1.0f,3.0f,5.0f,2.0f };
	Quaternion identity = IdentityQuaternion();
	Quaternion conj = Conjugate(q1);
	Quaternion inv = Inverse(q1);
	Quaternion normal = Normalize(q1);
	Quaternion mul1 = Multiply(q1, q2);
	Quaternion mul2 = Multiply(q2, q1);
	float norm = Norm(q1);

	ImGui::Begin("TitleScene");
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f     : Identity",
		identity.x, identity.y, identity.z, identity.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f  : Conjugate",
		conj.x, conj.y, conj.z, conj.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f  : Inverse",
		inv.x, inv.y, inv.z, inv.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f     : Normalize",
		normal.x, normal.y, normal.z, normal.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f  : Multiply(q1,q2)",
		mul1.x, mul1.y, mul1.z, mul1.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : Multiply(q2,q1)",
		mul2.x, mul2.y, mul2.z, mul2.w);
	ImGui::Text("%1.2f                    : Norm", norm);
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