#include "WorldTransform.h"

void WorldTransform::Initialize() {
	//CBVの作成
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(device, sizeof(ConstBuffDataWorldTransform));
}

void WorldTransform::TransferMatrix() {
	//Resourceに書き込む
	worldTransformData_ = static_cast<ConstBuffDataWorldTransform*>(constBuff_->Map());
	worldTransformData_->world = matWorld_;
	constBuff_->Unmap();
}

void WorldTransform::UpdateMatrixFromEuler() {
	//ワールド行列を計算
	matWorld_ = MakeAffineMatrix(scale_, rotation_, translation_);

	//親がいれば行列を掛ける
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}

	//ワールド行列を転送する
	WorldTransform::TransferMatrix();
}

void WorldTransform::UpdateMatrixFromQuaternion() {
	//ワールド行列を計算
	matWorld_ = MakeAffineMatrix(scale_, quaternion_, translation_);

	//親がいれば行列を掛ける
	if (parent_) {
		matWorld_ = Multiply(matWorld_, parent_->matWorld_);
	}

	//ワールド行列を転送する
	WorldTransform::TransferMatrix();
}

void WorldTransform::SetParent(const WorldTransform* parent) {
	//親を設定
	parent_ = parent;
	//親基準のローカル座標を計算
	if (parent_) {
		translation_ = Subtract(translation_, parent->translation_);
	}
}

void WorldTransform::UnsetParent() {
	if (parent_) {
		//新しいポジションを設定
		translation_ = { Vector3{matWorld_.m[3][0],matWorld_.m[3][1],matWorld_.m[3][2]} };
	}
	//親子付けを外す
	parent_ = nullptr;
}