#pragma once
#include "Engine/Base/Buffers/UploadBuffer.h"
#include "Engine/3D/Camera/Camera.h"
#include "EmitterBuilder.h"
#include <vector>

class ParticleSystem {
public:
	//パーティクルの最大数
	const uint32_t kMaxInstance = 1000;

	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	//マテリアルの構造体
	struct MaterialData {
		Vector4 color{};
		Matrix4x4 uvTransform{};
	};

	//GPUに送る構造体
	struct ParticleForGPU {
		Matrix4x4 world;
		Vector4 color;
	};

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

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
	/// <param name="viewProjection"></param>
	void Draw(const Camera& camera);

	/// <summary>
	/// エミッターの追加
	/// </summary>
	/// <param name="particleEmitter"></param>
	void AddParticleEmitter(ParticleEmitter* particleEmitter) { particleEmitters_.push_back(std::unique_ptr<ParticleEmitter>(particleEmitter)); };

	/// <summary>
	/// GPUハンドルを取得
	/// </summary>
	/// <returns></returns>
	const uint32_t& GetSrvIndex() const { return srvIndex_; };

	/// <summary>
	/// インスタンス数を取得
	/// </summary>
	/// <returns></returns>
	const uint32_t& GetNumInstance() const { return numInstance_; };

	/// <summary>
	/// エミッターを取得
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	ParticleEmitter* GetParticleEmitter(const std::string& name);

	/// <summary>
	/// エミッターのリストを取得
	/// </summary>
	/// <param name="name"></param>
	/// <returns></returns>
	std::list<ParticleEmitter*> GetParticleEmitters(const std::string& name);

private:
	/// <summary>
	/// 頂点バッファの作成
	/// </summary>
	void CreateVertexBuffer();

	/// <summary>
	/// マテリアル用のリソースを作成
	/// </summary>
	void CreateMaterialResource();

	/// <summary>
	/// Instancing用のリソースの作成
	/// </summary>
	void CreateInstancingResource();

	/// <summary>
	/// パーティクルのデータをGPUに転送
	/// </summary>
	void UpdateInstancingResource(const Camera& camera);

private:
	//インスタンス
	static ParticleSystem* instance_;
	//コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	//Instancing用のWorldTransform
	std::unique_ptr<UploadBuffer> instancingResource_ = nullptr;
	ParticleForGPU* instancingData_ = nullptr;
	//頂点バッファ
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;
	VertexData* vertexData_ = nullptr;
	//マテリアル用のリソース
	std::unique_ptr<UploadBuffer> materialResource_ = nullptr;
	MaterialData* materialData_ = nullptr;
	//頂点データ
	std::vector<VertexData> vertices_{};
	//頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//テクスチャハンドル
	uint32_t textureHandle_ = 0;
	//Instancing用のSRVの番号
	uint32_t srvIndex_ = 0;
	//エミッターのリスト
	std::list<std::unique_ptr<ParticleEmitter>> particleEmitters_{};
	//インスタンス数
	uint32_t numInstance_ = 0;
};

