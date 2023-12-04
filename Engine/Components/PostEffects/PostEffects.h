#pragma once
#include "Engine/Base/GraphicsCommon/GraphicsCommon.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include <vector>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

class PostEffects {
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	//DXC
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> blurRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> postEffectRootSignature_ = nullptr;
	//PipelineState
	std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> blurPipelineStates_{};
	std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> postEffectPipelineStates_{};
};

