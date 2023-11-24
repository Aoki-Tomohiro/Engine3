#include "FollowCamera.h"

void FollowCamera::Initialize() {
	//ビュープロジェクションの初期化
	camera_.Initialize();
};

void FollowCamera::Update() {

	//追従対象がいれば
	if (target_) {
		//追従座標の補間
		interTarget_ = Lerp(interTarget_, GetTargetWorldPosition(), 0.1f);
	}

	//追従対象からのオフセット
	Vector3 offset = FollowCamera::Offset();
	//カメラ座標
	camera_.translation_ = Add(interTarget_, offset);

	XINPUT_STATE joyState{};

	//旋回操作
	if (Input::GetInstance()->GetJoystickState(joyState)) {

		//しきい値
		const float threshold = 0.7f;

		//回転フラグ
		bool isRotation = false;

		//回転量
		Vector3 rotation = {
			(float)joyState.Gamepad.sThumbRY / SHRT_MAX,
			(float)joyState.Gamepad.sThumbRX / SHRT_MAX,
			0.0f
		};

		//スティックの押し込みが遊び範囲を超えていたら回転フラグをtureにする
		if (Length(rotation) > threshold) {
			isRotation = true;
		}

		if (isRotation) {
			//回転速度
			const float kRotSpeedX = 0.02f;
			const float kRotSpeedY = 0.04f;

			destinationAngleX_ -= rotation.x * kRotSpeedX;
			destinationAngleY_ += rotation.y * kRotSpeedY;
		}
	}

	camera_.rotation_.x = LerpShortAngle(camera_.rotation_.x, destinationAngleX_, 0.1f);
	camera_.rotation_.y = LerpShortAngle(camera_.rotation_.y, destinationAngleY_, 0.1f);

	//ビュー行列の更新
	camera_.UpdateMatrix();
};

Vector3 FollowCamera::Offset() {
	//追従対象からのオフセット
	Vector3 offset = { 0.0f, 2.0f, -20.0f };
	//回転行列の合成
	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(camera_.rotation_.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(camera_.rotation_.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(camera_.rotation_.z);
	Matrix4x4 rotateMatrix = Multiply(rotateXMatrix, Multiply(rotateYMatrix, rotateZMatrix));
	//オフセットをカメラの回転に合わせて回転させる
	offset = TransformNormal(offset, rotateMatrix);

	return offset;
}


void FollowCamera::Reset() {
	//追従対象がいれば
	if (target_) {
		//追従座標・角度の初期化
		interTarget_ = target_->translation_;
		camera_.rotation_.x = target_->rotation_.x;
		camera_.rotation_.y = target_->rotation_.y;
	}
	destinationAngleX_ = camera_.rotation_.x;
	destinationAngleY_ = camera_.rotation_.y;

	//追従対象からのオフセット
	Vector3 offset = FollowCamera::Offset();
	camera_.translation_ = Add(interTarget_, offset);
}

void FollowCamera::SetTarget(const WorldTransform* target) {
	target_ = target;
	FollowCamera::Reset();
}

Vector3 FollowCamera::GetTargetWorldPosition() {
	Vector3 pos{};
	pos.x = target_->matWorld_.m[3][0];
	pos.y = target_->matWorld_.m[3][1];
	pos.z = target_->matWorld_.m[3][2];
	return pos;
}