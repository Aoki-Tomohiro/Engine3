#pragma once
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Math/MathFunction.h"

class Sprite {
public:
	//頂点数
	static const int kMaxVertices = 6;

	/// <summary>
	/// 頂点データ
	/// </summary>
	struct VertexData {
		Vector4 position{};
		Vector2 texcoord{};
	};

	/// <summary>
	/// マテリアルデータ
	/// </summary>
	struct MaterialData {
		Vector4 color{};
		Matrix4x4 uvTransform{};
	};

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// スプライトの作成
	/// </summary>
	/// <param name="textureHandle"></param>
	/// <param name="position"></param>
	/// <returns></returns>
	static Sprite* Create(uint32_t textureHandle, Vector2 position);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <returns></returns>
	const Vector2& GetPosition() const { return position_; };

	/// <summary>
	/// 座標を取得
	/// </summary>
	/// <param name="position"></param>
	void SetPosition(const Vector2& position) { position_ = position; };

	/// <summary>
	/// 角度を取得
	/// </summary>
	/// <returns></returns>
	float GetRotation() const { return rotation_; };

	/// <summary>
	/// 角度を設定
	/// </summary>
	/// <param name="rotation"></param>
	void SetRotation(float rotation) { rotation_ = rotation; };

	/// <summary>
	/// 色を取得
	/// </summary>
	/// <returns></returns>
	const Vector4& GetColor() const { return color_; };

	/// <summary>
	/// 色を設定
	/// </summary>
	/// <param name="color"></param>
	void SetColor(const Vector4& color) { color_ = color; };

	/// <summary>
	/// サイズを取得
	/// </summary>
	/// <returns></returns>
	const Vector2& GetSize() const { return size_; };

	/// <summary>
	/// サイズを設定
	/// </summary>
	/// <param name="size"></param>
	void SetSize(const Vector2& size) { size_ = size; };

	/// <summary>
	/// アンカーポイントを取得
	/// </summary>
	/// <returns></returns>
	const Vector2& GetAnchorPoint() const { return anchorPoint_; };

	/// <summary>
	/// アンカーポイントを設定
	/// </summary>
	/// <param name="anchorPoint"></param>
	void SetAnchorPoint(const Vector2& anchorPoint) { anchorPoint_ = anchorPoint; };

	/// <summary>
	/// 左右フリップのフラグを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsFlipX() { return isFlipX_; };

	/// <summary>
	/// 左右フリップのフラグを設定
	/// </summary>
	/// <param name="isFlipX"></param>
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; };

	/// <summary>
	/// 上下フリップのフラグを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsFlipY() { return isFlipY_; };

	/// <summary>
	/// 上下フリップのフラグを設定
	/// </summary>
	/// <param name="isFlipX"></param>
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; };

	/// <summary>
	/// 非表示フラグを取得
	/// </summary>
	/// <returns></returns>
	bool GetIsInvisible() { return isInvisible_; };

	/// <summary>
	/// 非表示フラグを設定
	/// </summary>
	/// <param name="isInvisible"></param>
	void SetIsInvisible(bool isInvisible) { isInvisible_ = isInvisible; };

	/// <summary>
	/// テクスチャハンドルを取得
	/// </summary>
	/// <returns></returns>
	uint32_t GetTextureHandle() { return textureHandle_; };

	/// <summary>
	/// テクスチャハンドルを設定
	/// </summary>
	/// <param name="textureHandle"></param>
	void SetTextureHandle(uint32_t textureHandle) { textureHandle_ = textureHandle; };

	/// <summary>
	/// テクスチャ左上座標を取得
	/// </summary>
	/// <returns></returns>
	const Vector2& GetTextureLeftTop() const { return textureLeftTop_; };

	/// <summary>
	/// テクスチャ左上座標を設定
	/// </summary>
	/// <param name="textureLeftTop"></param>
	void SetTextureLeftTop(const Vector2& textureLeftTop) { textureLeftTop_ = textureLeftTop; };

	/// <summary>
	/// テクスチャ切り出しサイズを取得
	/// </summary>
	/// <returns></returns>
	const Vector2& GetTextureSize() const { return textureSize_; };

	/// <summary>
	/// テクスチャ切り出しサイズを設定
	/// </summary>
	/// <param name="textureSize"></param>
	void SetTextureSize(const Vector2& textureSize) { textureSize_ = textureSize; };

	/// <summary>
	/// UV座標を取得
	/// </summary>
	/// <returns></returns>
	const Vector2 GetUVTranslation() const { return uvTranslation_; };

	/// <summary>
	/// UV座標を設定
	/// </summary>
	/// <param name="uvTranslation"></param>
	void SetUVTranslation(const Vector2& uvTranslation) { uvTranslation_ = uvTranslation; };

	/// <summary>
	/// UV角度を取得
	/// </summary>
	/// <returns></returns>
	float GetUVRotation() { return uvRotation_; };

	/// <summary>
	/// UV角度を設定
	/// </summary>
	/// <param name="uvRotation"></param>
	void SetUVRotation(float uvRotation) { uvRotation_ = uvRotation; };

	/// <summary>
	/// UVスケールを取得
	/// </summary>
	/// <returns></returns>
	const Vector2 GetUVScale() const { return uvScale_; };

	/// <summary>
	/// UVスケールを設定
	/// </summary>
	/// <param name="uvScale"></param>
	void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; };

private:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(uint32_t textureHandle, Vector2 position);

	/// <summary>
	/// 頂点バッファの作成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// 頂点バッファの更新
	/// </summary>
	void UpdateVertexBuffer();

	/// <summary>
	/// マテリアル用のリソースの作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// マテリアルの更新
	/// </summary>
	void UpdateMaterial();

	/// <summary>
	/// WVP用のリソースの作成
	/// </summary>
	void CreateWVPResource();

	/// <summary>
	/// 行列の計算・転送
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// テクスチャサイズをイメージに合わせる
	/// </summary>
	void AdjustTextureSize();

private:
	//射影行列
	static Matrix4x4 sMatProjection_;
	//頂点バッファ
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;
	VertexData* vertexData_ = nullptr;
	//マテリアル用のリソース
	std::unique_ptr<UploadBuffer> materialResource_ = nullptr;
	MaterialData* materialData_ = nullptr;
	//WVP用のリソース
	std::unique_ptr<UploadBuffer> wvpResource_ = nullptr;
	Matrix4x4* wvpData_ = nullptr;
	//頂点データ
	std::array<VertexData,kMaxVertices> vertices_{};
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//テクスチャの情報
	D3D12_RESOURCE_DESC resourceDesc_{};
	//座標
	Vector2 position_ = { 0.0f,0.0f };
	//回転
	float rotation_ = 0.0f;
	//スケール
	Vector2 scale_ = { 1.0f,1.0f };
	//色
	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };
	//サイズ
	Vector2 size_ = { 100.0f,100.0f };
	//アンカーポイント
	Vector2 anchorPoint_{ 0.0f,0.0f };
	//左右フリップ
	bool isFlipX_ = false;
	//上下フリップ
	bool isFlipY_ = false;
	//非表示フラグ
	bool isInvisible_ = false;
	//テクスチャ左上座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	//テクスチャ切り出しサイズ
	Vector2 textureSize_ = { 100.0f,100.0f };
	//uv座標
	Vector2 uvTranslation_ = { 0.0f,0.0f };
	//uv角度
	float uvRotation_ = 0.0f;
	//uvスケール
	Vector2 uvScale_ = { 1.0f,1.0f };
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
};