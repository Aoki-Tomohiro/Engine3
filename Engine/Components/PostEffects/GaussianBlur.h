#pragma once
#include "Engine/Base/Renderer/Renderer.h"

class GaussianBlur {
public:
	enum BlurState {
		kHorizontal,
		kVertical,
		kCountOfBlur,
	};

	struct BlurData {
		int32_t textureWidth;
		int32_t textureHeight;
		float padding[2];
		float weight[8];
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 対象のリソースを取得
	/// </summary>
	void SetColorBuffer(const ColorBuffer* colorBuffer) { colorBuffer_ = colorBuffer; };

	/// <summary>
	/// 横ぼかしのリソースを取得
	/// </summary>
	/// <returns></returns>
	const ColorBuffer* GetHorizontalBlurResource() const { return blurColorBuffer_[kHorizontal].get(); };

	/// <summary>
	/// 縦ぼかしのリソースを取得
	/// </summary>
	/// <returns></returns>
	const ColorBuffer* GetVerticalBlurResource() const { return blurColorBuffer_[kVertical].get(); };

private:
	/// <summary>
	/// ぼかし用のCBVを作成
	/// </summary>
	void CreateBlurUploadBuffer();

	/// <summary>
	/// 縮小ぼかし用のCBVを作成
	/// </summary>
	void CreateShrinkBlurUploadBuffer();

	/// <summary>
	/// PSOの作成
	/// </summary>
	void CreatePipelineState();

private:
	//GraphicsCommon
	GraphicsCommon* graphicsCommon_ = nullptr;
	//レンダラー
	Renderer* renderer_ = nullptr;
	//RootSignature
	std::unique_ptr<RootSignature> rootSignature_ = nullptr;
	//PipelineState
	std::vector<std::unique_ptr<PipelineState>> pipelineStates_{};
	//書き込むリソース
	std::vector<std::unique_ptr<ColorBuffer>> blurColorBuffer_{};
	//ブラー用のCBV
	std::unique_ptr<UploadBuffer> blurUploadBuffer_ = nullptr;
	//書き込み用
	BlurData* blurData_ = nullptr;
	//縮小ぼかし用のCBV
	std::unique_ptr<UploadBuffer> shrinkBlurUploadBuffer_ = nullptr;
	//書き込み用
	BlurData* shrinkBlurData_ = nullptr;
	//対象のリソース
	const GpuResource* colorBuffer_ = nullptr;
};

