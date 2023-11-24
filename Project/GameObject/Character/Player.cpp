#include "Player.h"
#include "Engine/Utilities/GlobalVariables.h"
#include <cassert>
#include <numbers>

void Player::Initialize(const std::vector<Model*>& models) {
	//基底クラスの初期化
	BaseCharacter::Initialize(models);
	//入力クラスのインスタンスを取得
	input_ = Input::GetInstance();
	//武器の作成
	weapon_ = std::make_unique<Weapon>();
	weapon_->Initialize(models_[1]);
	weapon_->SetParent(&worldTransform_);
	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);

	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	//グループを追加
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "BehaviorDashTime", behaviorDashTime_);
}

void Player::Update() {
	//Behaviorの遷移処理
	if (behaviorRequest_) {
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}


	//Behaviorの実行
	switch (behavior_) {
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	}

	//当たっていないときは落ちる
	if (onCollision_ == false) {
		worldTransform_.translation_.y -= 0.1f;
	}

	//落ちたらリスタート
	if (worldTransform_.translation_.y <= -10.0f) {
		Restart();
	}

	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Slerp(worldTransform_.quaternion_, moveQuaternion_, 0.4f);
	worldTransform_.UpdateMatrix(RotationType::Quaternion);

	//武器の更新
	weapon_->Update();

	//次のフレーム用のフラグを保存
	preOnCollision_ = onCollision_;
	onCollision_ = false;

	//グローバル変数の適応
	Player::ApplyGlobalVariables();
}

void Player::Draw(const Camera& camera) {
	//モデルの描画
	models_[0]->Draw(worldTransform_, camera);
	if (behavior_ == Behavior::kAttack) {
		weapon_->Draw(camera);
	}
}

void Player::Restart() {
	//初期座標に戻す
	worldTransform_.translation_ = { 0.0f,0.0f,0.0f };
	worldTransform_.parent_ = nullptr;
}

void Player::OnCollision(Collider* collider) {
	//床と当たっていた場合
	if (collider->GetCollisionAttribute() & kCollisionAttributeFloor) {
		//衝突フラグを立てる
		onCollision_ = true;
		//親を設定する
		parent_ = &collider->GetWorldTransform();
		//現在の親と別の親なら親子付けする
		if (worldTransform_.parent_ != parent_) {
			worldTransform_.UnsetParent();
			worldTransform_.SetParent(parent_);
		}
	}

	//敵と当たったらリスタート
	if (collider->GetCollisionAttribute() & kCollisionAttributeEnemy) {
		Restart();
	}

	//ゴールに触れたらリスタート
	if (collider->GetCollisionAttribute() & kCollisionAttributeGoal) {
		Restart();
	}
}

Vector3 Player::GetWorldPosition() {
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1] + 1.0f;
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}

void Player::BehaviorRootInitialize() {

}

void Player::BehaviorRootUpdate() {
	//ダッシュのクールタイム
	const uint32_t behaviorDashCoolTime = 60;

	XINPUT_STATE joyState{};

	if (input_->GetJoystickState(joyState)) {

		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isMoving = false;

		//移動量
		velocity_ = {
			(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
			0.0f,
			(float)joyState.Gamepad.sThumbLY / SHRT_MAX,
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Length(velocity_) > threshold) {
			isMoving = true;
		}

		if (isMoving) {

			//速さ
			const float speed = 0.3f;

			//移動量に速さを反映
			velocity_ = Multiply(Normalize(velocity_), speed);

			//移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotateMatrix = MakeRotateYMatrix(camera_->rotation_.y);
			velocity_ = TransformNormal(velocity_, rotateMatrix);

			//移動
			worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);

			//回転
			Vector3 nVelocity_ = Normalize(velocity_);
			Vector3 cross = Normalize(Cross({ 0.0f,0.0f,1.0f }, nVelocity_));
			float dot = Dot({ 0.0f,0.0f,1.0f }, nVelocity_);
			moveQuaternion_ = MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
		}
	}

	//攻撃行動に変更
	if (input_->GetJoystickState(joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
			if (workDash_.coolTime == behaviorDashCoolTime) {
				behaviorRequest_ = Behavior::kAttack;
			}
		}
	}

	//ダッシュ行動に変更
	if (workDash_.coolTime != behaviorDashCoolTime) {
		workDash_.coolTime++;
	}

	if (input_->GetJoystickState(joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_X) {
			if (workDash_.coolTime == behaviorDashCoolTime) {
				behaviorRequest_ = Behavior::kDash;
			}
		}
	}

	//ジャンプ行動に変更
	if (input_->GetJoystickState(joyState)) {
		if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A) {
			behaviorRequest_ = Behavior::kJump;
		}
	}
}

void Player::BehaviorDashInitialize() {
	workDash_.dashParameter_ = 0;
	workDash_.coolTime = 0;
	worldTransform_.rotation_.y = destinationAngleY_;
}

void Player::BehaviorDashUpdate() {

	XINPUT_STATE joyState{};

	if (input_->GetJoystickState(joyState)) {
		//速さ
		float kSpeed = 1.0f;
		//移動量
		Vector3 move = {
			(float)joyState.Gamepad.sThumbLX / SHRT_MAX,
			0.0f,
			(float)joyState.Gamepad.sThumbLY / SHRT_MAX,
		};

		//移動量に速さを反映
		move = Multiply(Normalize(move), kSpeed);

		//移動ベクトルをカメラの角度だけ回転する
		Matrix4x4 rotateMatrix = MakeRotateYMatrix(camera_->rotation_.y);
		move = TransformNormal(move, rotateMatrix);

		//移動
		worldTransform_.translation_ = Add(worldTransform_.translation_, move);
	}

	//規定の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= behaviorDashTime_) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorAttackInitialize() {
	//攻撃開始
	weapon_->Attack();
}

void Player::BehaviorAttackUpdate() {
	//攻撃が終わったら通常状態に戻す
	if (weapon_->GetIsAttack() == false) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorJumpInitialize() {
	worldTransform_.translation_.y = 0.0f;
	const float kJumpFirstSpeed = 1.0f;
	velocity_.y = kJumpFirstSpeed;
	worldTransform_.UnsetParent();
}

void Player::BehaviorJumpUpdate() {
	worldTransform_.translation_ = Add(worldTransform_.translation_, velocity_);
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_ = Add(velocity_, accelerationVector);

	if (worldTransform_.parent_) {
		behaviorRequest_ = Behavior::kRoot;
		worldTransform_.translation_.y = 0.0f;
	}
}

void Player::ApplyGlobalVariables() {
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	behaviorDashTime_ = globalVariables->GetIntValue(groupName, "BehaviorDashTime");
}