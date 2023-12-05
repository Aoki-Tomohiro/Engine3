#pragma once
#include "Engine/Base/Buffer/UploadBuffer.h"
#include "Engine/Math/MathFunction.h"
#include <memory>

struct ConstBuffDataWorldTransform {
	Matrix4x4 world;
};

class WorldTransform {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ワールド行列を転送
	/// </summary>
	void TransferMatrix();

	/// <summary>
	/// オイラー角で行列の計算・転送
	/// </summary>
	void UpdateMatrixFromEuler();

	/// <summary>
	/// クォータニオンで行列の計算・転送
	/// </summary>
	void UpdateMatrixFromQuaternion();

	/// <summary>
	/// 親を設定
	/// </summary>
	/// <param name="parent"></param>
	void SetParent(const WorldTransform* parent);

	/// <summary>
	/// 親子付けを解除する
	/// </summary>
	void UnsetParent();

	/// <summary>
	/// CBVを取得
	/// </summary>
	/// <returns></returns>
	UploadBuffer* GetConstantBuffer() const { return constBuff_.get(); };

private:
	//CBV
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;
	//書き込み用
	ConstBuffDataWorldTransform* worldTransformData_ = nullptr;

public:
	//スケール
	Vector3 scale_ = { 1.0f,1.0f,1.0f };
	//角度
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };
	//座標
	Vector3 translation_ = { 0.0f,0.0f,0.0f };
	//クォータニオン
	Quaternion quaternion_{ 0.0f,0.0f,0.0f,1.0f };
	//ワールド行列
	Matrix4x4 matWorld_{};
	//親となるワールド変換へのポインタ
	const WorldTransform* parent_ = nullptr;
};