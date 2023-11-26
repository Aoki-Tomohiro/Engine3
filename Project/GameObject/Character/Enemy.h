#pragma once
#include "BaseCharacter.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Components/CollisionManager/Collider.h"

class Enemy : public BaseCharacter, public Collider {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="models"></param>
	void Initialize(const std::vector<Model*>& models) override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	void Draw(const Camera& camera) override;

	/// <summary>
	/// 浮遊ギミックの初期化
	/// </summary>
	void InitializeFloatingGimmick();

	/// <summary>
	/// 浮遊ギミック更新
	/// </summary>
	void UpdateFloatingGimmick();

	/// <summary>
	/// 中心座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetCenterPosition() const;

	/// <summary>
	/// 親を設定
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const WorldTransform* parent) { worldTransform_.parent_ = parent; };

	/// <summary>
	/// 死亡フラグを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsDead() const { return isDead_; };

	/// <summary>
	/// プレイヤーの攻撃フラグを取得
	/// </summary>
	/// <param name="comboIndex"></param>
	void SetIsPlayerAttack(bool isAttack) { isPlayerAttack_ = isAttack; };

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
	//ワールドトランスフォーム
	WorldTransform worldTransformBody_;
	WorldTransform worldTransformL_arm_;
	WorldTransform worldTransformR_arm_;
	//浮遊ギミックの媒介変数
	float floatingParameter_ = 0.0f;
	//浮遊移動のサイクル<frame>
	int32_t cycle_ = 60;
	//浮遊の振動<m>
	float amplitude_ = 0.1f;
	//移動速度
	Vector3 velocity_{ 0.1f,0.0f,0.0f };
	bool isDead_ = false;
	//親のポインタ
	const WorldTransform* parent_ = nullptr;
	//当たり判定のフラグ
	bool onCollision_ = false;
	bool preOnCollision_ = false;
	//プレイヤーの攻撃フラグ
	bool isPlayerAttack_ = false;
	//攻撃の当たった数
	uint32_t hitCount_ = 0;
};