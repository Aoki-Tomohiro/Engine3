#include "Camera.h"

void Camera::Initialize() {
	//CBVの作成
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataViewProjection));
}

void Camera::TransferMatrix() {
	//Resourceに書き込む
	viewProjectionData_ = static_cast<ConstBuffDataViewProjection*>(constBuff_->Map());
	viewProjectionData_->view = matView_;
	viewProjectionData_->projection = matProjection_;
	constBuff_->Unmap();
}

void Camera::UpdateViewMatrixFromEuler() {
	//平行移動行列の計算
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);
	//回転行列の計算
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotation_.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotation_.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotation_.z);
	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	//ビュー行列の計算
	matView_ = Multiply(Inverse(translateMatrix), Inverse(rotateMatrix));
}

void Camera::UpdateViewMatrixFromQuaternion() {
	//平行移動行列の計算
	Matrix4x4 translateMatrix = MakeTranslateMatrix(translation_);
	//回転行列の計算
	Matrix4x4 rotateMatrix = MakeRotateMatrix(quaternion_);
	//ビュー行列の計算
	matView_ = Multiply(Inverse(translateMatrix), Inverse(rotateMatrix));
}

void Camera::UpdateProjectionMatrix() {
	//プロジェクション行列の計算
	matProjection_ = MakePerspectiveFovMatrix(fov_, aspectRatio_, nearClip_, farClip_);
}

void Camera::UpdateMatrixFromEuler() {
	//ビュー行列の計算
	Camera::UpdateViewMatrixFromEuler();
	//プロジェクション行列の計算
	Camera::UpdateProjectionMatrix();
	//ビュープロジェクションを転送する
	Camera::TransferMatrix();
}

void Camera::UpdateMatrixFromQuaternion() {
	//ビュー行列の計算
	Camera::UpdateViewMatrixFromQuaternion();
	//プロジェクション行列の計算
	Camera::UpdateProjectionMatrix();
	//ビュープロジェクションを転送する
	Camera::TransferMatrix();
}