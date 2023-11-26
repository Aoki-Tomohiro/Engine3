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

	//パーティクル
	particleSystem_ = std::make_unique<ParticleSystem>();
	particleSystem_->Initialize();
}

void Weapon::Update() {
	//前のフレームの当たり判定のフラグを取得
	preOnCollision_ = onCollision_;
	onCollision_ = false;

	//当たり判定の位置を決める
	Vector3 direction{ 0.0f,0.0f,4.0f };
	//direction = TransformNormal(direction, worldTransform_.matWorld_);
	direction = TransformNormal(direction, worldTransform_.parent_->matWorld_);
	worldTransformCollision_.translation_ = { worldTransform_.matWorld_.m[3][0],worldTransform_.matWorld_.m[3][1] ,worldTransform_.matWorld_.m[3][2] };
	worldTransformCollision_.translation_ = Add(worldTransformCollision_.translation_, direction);

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
	worldTransformCollision_.UpdateMatrixFromEuler();

	//パーティクルの更新
	particleSystem_->Update();
}

void Weapon::Draw(const Camera& camera) {
	//モデルの描画
	model_->Draw(worldTransform_, camera);
}

void Weapon::DrawParticle(const Camera& camera) {
	particleSystem_->Draw(camera);
}

void Weapon::OnCollision(Collider* collider) {
	onCollision_ = true;
	if (onCollision_ != preOnCollision_) {
		//パーティクル
		ParticleEmitter* newParticleEmitter = EmitterBuilder()
			.SetArea({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetAzimuth(0.0f, 360.0f)
			.SetColor({ 1.0f, 0.5f, 0.2f, 1.0f }, { 1.0f, 0.8f, 0.4f, 1.0f })
			.SetCount(100)
			.SetDeleteTime(3.0f)
			.SetElevation(0.0f, 180.0f)
			.SetEmitterName("Hit")
			.SetFrequency(4.0f)
			//.SetLifeTime(0.1f, 1.0f)
			.SetLifeTime(0.2f, 0.4f)
			.SetRotation({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetScale({ 0.2f, 0.2f,0.2f }, { 0.25f ,0.25f ,0.25f })
			.SetTranslation(collider->GetWorldPosition())
			//.SetVelocity({ 0.02f ,0.02f ,0.02f }, { 0.04f ,0.04f ,0.04f })
			.SetVelocity({ 0.2f, 0.2f, 0.2f }, { 0.4f, 0.4f, 0.4f })
			.Build();
		particleSystem_->AddParticleEmitter(newParticleEmitter);
	}
}

Vector3 Weapon::GetWorldPosition() {
	Vector3 pos{};
	pos.x = worldTransformCollision_.matWorld_.m[3][0];
	pos.y = worldTransformCollision_.matWorld_.m[3][1];
	pos.z = worldTransformCollision_.matWorld_.m[3][2];
	return pos;
}