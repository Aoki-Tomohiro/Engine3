#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Matrix/WorldTransform.h"
#include "Engine/Components/CollisionManager/Collider.h"

/// <summary>
/// 武器クラス
/// </summary>
class Weapon : public Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="model"></param>
	void Initialize(Model* model);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera& camera);

	/// <summary>
	/// 親を設定
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; };

	/// <summary>
	/// 攻撃フラグを取得
	/// </summary>
	bool GetIsAttack() { return isAttack_; };

	/// <summary>
	/// 
	/// </summary>
	/// <param name="isAttack"></param>
	void SetIsAttack(bool isAttack) { isAttack_ = isAttack; };

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="translation"></param>
	void SetTranslation(const Vector3& translation) { worldTransform_.translation_ = translation; };

	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(const Vector3& rotation) { worldTransform_.rotation_ = rotation; };

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns></returns>
	WorldTransform& GetWorldTransform() override { return worldTransform_; };

	/// <summary>
	/// 衝突判定
	/// </summary>
	/// <param name="collider"></param>
	void OnCollision(Collider* collider) override;

	/// <summary>
	/// ワールドポジションを取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition() override;

private:
	//モデル
	Model* model_ = nullptr;
	//ワールドトランスフォーム
	WorldTransform worldTransform_{};
	WorldTransform worldTransformCollision_{};
	//当たり判定のフラグ
	bool isAttack_ = false;
};
