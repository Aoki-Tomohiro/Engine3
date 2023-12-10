#pragma once
#include "Engine/Base/Graphics/GraphicsCommon.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include "Engine/Base/PipelineState/PipelineState.h"
#include <dxcapi.h>
#include <vector>
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

	/// <summary>
	/// シェーダー読み込み
	/// </summary>
	/// <param name="filePath"></param>
	/// <param name="profile"></param>
	/// <returns></returns>
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(const std::wstring& filePath, const wchar_t* profile);

private:
	Renderer() = default;
	~Renderer() = default;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	//ディレクトリパス
	static const std::wstring& kBaseDirectory;

	/// <summary>
	/// DXCの初期化
	/// </summary>
	void InitializeDXC();

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

	//GraphicsCommon
	GraphicsCommon* graphicsCommon_ = nullptr;

	//DXCCompiler
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils_ = nullptr;

	Microsoft::WRL::ComPtr<IDxcCompiler3> dxcCompiler_ = nullptr;

	Microsoft::WRL::ComPtr<IDxcIncludeHandler> includeHandler_ = nullptr;

	//RootSignature
	std::unique_ptr<RootSignature> modelRootSignature_ = nullptr;

	std::unique_ptr<RootSignature> spriteRootSignature_ = nullptr;

	std::unique_ptr<RootSignature> particleRootSignature_ = nullptr;

	//PipelineState
	std::vector<std::unique_ptr<PipelineState>> modelPipelineStates_{};

	std::vector<std::unique_ptr<PipelineState>> spritePipelineStates_{};

	std::vector<std::unique_ptr<PipelineState>> particlePipelineStates_{};

	//Resource
	std::unique_ptr<ColorBuffer> sceneColorBuffer_ = nullptr;

	std::unique_ptr<DepthBuffer> sceneDepthBuffer_ = nullptr;
};