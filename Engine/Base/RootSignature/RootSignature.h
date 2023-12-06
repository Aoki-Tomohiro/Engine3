#pragma once
#include "Engine/Base/Graphics/GraphicsCommon.h"

class RootSignature {
public:
	/// <summary>
	/// ルートパラメーターを設定
	/// </summary>
	/// <param name="parameter"></param>
	void AddRootParameter(const D3D12_ROOT_PARAMETER& rootParameter);

	/// <summary>
	/// サンプラーを設定
	/// </summary>
	/// <param name="sampler"></param>
	void AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& staticSamplers);

	/// <summary>
	/// 作成
	/// </summary>
	void Finalize();

	/// <summary>
	/// ルートシグネチャを取得
	/// </summary>
	/// <returns></returns>
	ID3D12RootSignature* GetRootSignature() const { return rootSignature_.Get(); };

private:
	//RootSignature
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	//RootParameter
	std::vector<D3D12_ROOT_PARAMETER> rootParameters_{};
	//Sampler
	std::vector<D3D12_STATIC_SAMPLER_DESC> staticSamplers_{};
};

