#include "LockOn.h"

void LockOn::Initialize() {
	//インスタンスを取得
	input_ = Input::GetInstance();
	//テクスチャ読み込み
	textureHandle_ = TextureManager::Load("Resources/Images/Reticle.png");
	//スプライトの生成
	lockOnMark_.reset(Sprite::Create(textureHandle_, { 0.0f,0.0f }));
	lockOnMark_->SetAnchorPoint({ 0.5f,0.5f });
}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies,const Camera& camera) {
	//ロックオン状態なら
	if (target_) {
		//ロックオン解除処理
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_THUMB)) {
			//ロックオンを外す
			target_ = nullptr;
		}
		else if (InRange(camera)) {
			//ロックオンを外す
			target_ = nullptr;
		}
	}
	else {
		//ロックオン対象の検索
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_THUMB)) {
			//目標
			std::list<std::pair<float, const Enemy*>> targets;

			//すべての敵に対して順にロックオン判定
			for (const std::unique_ptr<Enemy>& enemy : enemies) {
				//敵のロックオン座標取得
				Vector3 positionWorld = enemy->GetCenterPosition();
				//ワールド→ビュー座標変換
				Vector3 positionView = Transform(positionWorld, camera.matView_);

				//距離条件チェック
				if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
					//カメラ前方との角度を計算
					//float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);
					float norm = Length(positionView);
					float arcTangent = std::acos(positionView.z / norm);

					//角度条件チェック(コーンに収まっているか)
					if (std::abs(arcTangent) <= angleRange_) {
						targets.emplace_back(std::make_pair(positionView.z, enemy.get()));
					}
				}

				//ターゲット対象をリセット
				target_ = nullptr;
				if (targets.size() != 0) {
					//距離で昇順にソート
					targets.sort([](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
					//ソートの結果一番近い敵をロックオン対象とする
					target_ = targets.front().second;
				}
			}
		}
	}

	//ロックオン状態なら
	if (target_) {
		//ロックオンマークの座標計算
		Vector3 positionWorld = target_->GetCenterPosition();
		// ビューポート行列
		Matrix4x4 matViewport = MakeViewportMatrix(0, 0, Application::kClientWidth, Application::kClientHeight, 0, 1);
		// ビュー行列とプロジェクション行列、ビューポート行列を合成する
		Matrix4x4 matViewProjectionViewport = Multiply(camera.matView_, Multiply(camera.matProjection_, matViewport));
		//ワールド座標からスクリーン座標に変換
		Vector3 positionScreen = Transform(positionWorld, matViewProjectionViewport);
		//Vector2に格納
		Vector2 positionScreenV2 = { positionScreen.x,positionScreen.y };
		//スプライトの座標を設定
		lockOnMark_->SetPosition(positionScreenV2);
	}
}

void LockOn::Draw() {
	if (target_) {
		lockOnMark_->Draw();
	}
}

bool LockOn::InRange(const Camera& camera) {
	//敵のロックオン座標取得
	Vector3 positionWorld = target_->GetCenterPosition();
	//ワールド→ビュー座標変換
	Vector3 positionView = Transform(positionWorld, camera.matView_);

	//距離条件チェック
	if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
		//カメラ前方との角度を計算
		float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);

		//角度条件チェック(コーンに収まっているか)
		if (std::abs(arcTangent) <= angleRange_) {
			return false;
		}
	}

	return true;
}

Vector3 LockOn::GetTargetPosition() const {
	if (target_) {
		return target_->GetCenterPosition();
	}
	return Vector3();
}