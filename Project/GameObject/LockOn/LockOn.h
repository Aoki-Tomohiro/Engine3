#pragma once
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Components/Input/Input.h"
#include "Project/GameObject/Character/Enemy.h"

class LockOn {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(const std::list<std::unique_ptr<Enemy>>& enemies, const Camera& camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ロックオン対象の座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetTargetPosition() const;

	/// <summary>
	/// ロックオン中かどうか
	/// </summary>
	/// <returns></returns>
	bool ExistTarget() const { return target_ ? true : false; };

private:
	/// <summary>
	/// 範囲外判定
	/// </summary>
	/// <param name="camera"></param>
	/// <returns></returns>
	bool InRange(const Camera& camera);

private:
	//インプット
	Input* input_ = nullptr;
	//ロックオンマーク用スプライト
	std::unique_ptr<Sprite> lockOnMark_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_ = 0u;
	//ロックオン対象
	const Enemy* target_ = nullptr;
	//最小距離
	float minDistance_ = 10.0f;
	//最大距離
	float maxDistance_ = 50.0f;
	//角度範囲
	const float kDegreeToRadian = 3.14159265358979323846f / 180.0f;
	float angleRange_ = 20.0f * kDegreeToRadian;
};

