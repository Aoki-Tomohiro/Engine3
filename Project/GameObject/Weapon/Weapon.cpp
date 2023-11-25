#include "Weapon.h"
#include "Engine/Base/ImGuiManager/ImGuiManager.h"
#include <cassert>

void Weapon::Initialize(Model* model) {
	//モデルの初期化
	assert(model);
	model_ = model;
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.translation_.y = 0.8f;
	worldTransformCollision_.Initialize();
	worldTransformCollision_.translation_.y = 0.8f;
	//worldTransformCollision_.scale_ = { 0.2f,0.6f,1.8f };
	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributeWeapon);
	SetCollisionMask(kCollisionMaskWeapon);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	AABB aabbSize = {
		{-worldTransformCollision_.scale_.x,-worldTransformCollision_.scale_.y,-worldTransformCollision_.scale_.z},
		{worldTransformCollision_.scale_.x,worldTransformCollision_.scale_.y,worldTransformCollision_.scale_.z} };
	SetAABB(aabbSize);
}

void Weapon::Update() {
	//当たり判定の位置を決める
	Vector3 direction{ 0.0f,0.0f,4.0f };
	//direction = TransformNormal(direction, worldTransform_.matWorld_);
	direction = TransformNormal(direction, worldTransform_.parent_->matWorld_);
	worldTransformCollision_.translation_ = { worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1] ,worldTransform_.matWorld_.m[3][2] };
	worldTransformCollision_.translation_ = Add(worldTransformCollision_.translation_, direction);

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrix(RotationType::Euler);
	worldTransformCollision_.UpdateMatrix(RotationType::Euler);
}

void Weapon::Draw(const Camera& camera) {
	//モデルの描画
	model_->Draw(worldTransform_, camera);
}

void Weapon::OnCollision(Collider* collider) {

}

Vector3 Weapon::GetWorldPosition() {
	Vector3 pos{};
	pos.x = worldTransformCollision_.matWorld_.m[3][0];
	pos.y = worldTransformCollision_.matWorld_.m[3][1];
	pos.z = worldTransformCollision_.matWorld_.m[3][2];
	return pos;
}