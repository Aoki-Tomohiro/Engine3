#include "Skydome.h"
#include <cassert>

void Skydome::Initialize(Model* model) {
	// NULLポインタチェック
	assert(model);
	model_ = model;
	worldTransform_.Initialize();
};

void Skydome::Update() {
	// 行列を定数バッファに転送
	worldTransform_.UpdateMatrix(RotationType::Euler);
};

void Skydome::Draw(const Camera& camera) {
	model_->Draw(worldTransform_, camera);
};