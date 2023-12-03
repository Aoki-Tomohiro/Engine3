#pragma once
#include "Engine/Base/GraphicsCommon/GraphicsCommon.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include <vector>
#include <dxcapi.h>
#pragma comment(lib,"dxcompiler.lib")

class Renderer {
public:
	enum ModelRenderingType {
		//不透明
		Opaque,
		//透明
		Transparent,
		//利用してはいけない
		RenderingTypeCount
	};

	enum BlendMode {
		//ブレンドなし
		kBlendModeNone,
		//通常aブレンド
		kBlendModeNormal,
		//加算
		kBlendModeAdd,
		//減算
		kBlendModeSubtract,
		//乗算
		kBlendModeMultiply,
		//スクリーン
		kBlendModeScreen,
		//利用してはいけない
		kCountOfBlendMode,
	};

	/// <summary>
	/// シングルトンインスタンスを取得
	/// </summary>
	/// <returns></returns>
	static Renderer* GetInstance();

	/// <summary>
	/// シングルトンインスタンスを削除
	/// </summary>
	static void DeleteInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// モデル描画前処理
	/// </summary>
	void PreDrawModels(ModelRenderingType type);

	/// <summary>
	/// モデル描画後処理
	/// </summary>
	void PostDrawModels();

	/// <summary>
	/// スプライト描画前処理
	/// </summary>
	void PreDrawSprites(BlendMode blendMode);

	/// <summary>
	/// スプライト描画後処理
	/// </summary>
	void PostDrawSprites();

	/// <summary>
	/// パーティクル描画前処理
	/// </summary>
	void PreDrawParticles();

	/// <summary>
	/// パーティクル描画後処理
	/// </summary>
	void PostDrawParticles();

private:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	/// <summary>
	/// DXCの初期化
	/// </summary>
	void InitializeDXC();

	/// <summary>
	/// シェーダーコンパイル
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	/// <summary>
	/// モデル用のPSOを作成
	/// </summary>
	void CreateModelPipelineState();

	/// <summary>
	/// スプライト用のPSOを作成
	/// </summary>
	void CreateSpritePipelineState();

	/// <summary>
	/// パーティクル用のPSOを作成
	/// </summary>
	void CreateParticlePipelineState();
	
private:
	//インスタンス
	static Renderer* instance_;
	//デバイス
	ID3D12Device* device_ = nullptr;
	//コマンドリスト
	ID3D12GraphicsCommandList* commandList_ = nullptr;
	//DXC
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;
	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> modelRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> spriteRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> particleRootSignature_ = nullptr;
	//PipelineState
	std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> modelPipelineStates_{};
	std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> spritePipelineStates_{};
	std::vector<Microsoft::WRL::ComPtr<ID3D12PipelineState>> particlePipelineStates_{};
};

