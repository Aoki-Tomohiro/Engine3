#pragma once
#include "Engine/Components/PostEffects/GaussianBlur.h"

class PostEffects {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

private:
	//レンダラー
	Renderer* renderer_ = nullptr;
	//ガウシアンブラー
	std::unique_ptr<GaussianBlur> gaussianBlur_ = nullptr;
	//RootSignature
	std::unique_ptr<RootSignature> rootSignature_ = nullptr;
	//PipelineState
	std::vector<std::unique_ptr<PipelineState>> pipelineStates_{};
};

