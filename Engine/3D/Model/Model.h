#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Engine/3D/Lights/DirectionalLight.h"
#include "Engine/3D/Matrix/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"

class Model {
public:
	/// <summary>
	/// ルートパラメータの番号
	/// </summary>
	enum class RootParameterIndex {
		Material,
		WorldlTransform,
		Camera,
		Texture,
		DirectionalLight
	};

	/// <summary>
	/// マテリアルデータ構造体
	/// </summary>
	struct MaterialData {
		std::string textureFilePath;
	};

	/// <summary>
	/// モデルデータ構造体
	/// </summary>
	struct ModelData {
		std::vector<Mesh::VertexData> vertices;
		MaterialData material;
		std::string name;
	};

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

	/// <summary>
	/// モデルの作成
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	static Model* CreateFromOBJ(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform"></param>
	/// <param name="viewProjection"></param>
	void Draw(const WorldTransform& worldTransform, const Camera& camera);

	/// <summary>
	/// 描画(テクスチャ張替え)
	/// </summary>
	/// <param name="worldTransform"></param>
	/// <param name="viewProjection"></param>
	/// <param name="textureHandle"></param>
	void Draw(const WorldTransform& worldTransform, const Camera& camera, uint32_t textureHandle);

	/// <summary>
	/// DirectionalLightを取得
	/// </summary>
	/// <returns></returns>
	DirectionalLight* GetDirectionalLight() { return directionalLight_.get(); };

	/// <summary>
	/// マテリアルを取得
	/// </summary>
	/// <returns></returns>
	Material* GetMaterial() { return material_.get(); };

private:
	/// <summary>
	/// Objファイルの読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	/// <summary>
	/// mtlファイルの読み込み
	/// </summary>
	/// <param name="directoryPath"></param>
	/// <param name="filename"></param>
	/// <returns></returns>
	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

private:
	//コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
	//モデルデータ
	static std::vector<ModelData> sModelDatas_;
	//頂点データ
	std::unique_ptr<Mesh> mesh_ = nullptr;
	//マテリアルデータ
	std::unique_ptr<Material> material_ = nullptr;
	//DirectionalLight
	std::unique_ptr<DirectionalLight> directionalLight_ = nullptr;
	//テクスチャハンドル
	uint32_t textureHandle_{};
};