#include "Model.h"
#include "Engine/Base/Graphics/GraphicsContext.h"
#include "Engine/Base/TextureManager/TextureManager.h"
#include <cassert>
#include <fstream>
#include <sstream>

//実体定義
std::vector<Model::ModelData> Model::sModelDatas_{};

void Model::StaticInitialize() {

}

Model* Model::CreateFromOBJ(const std::string& directoryPath, const std::string& filename) {
	//モデルを生成
	Model* model = new Model();

	//同じモデルがないか探す
	for (ModelData modelData : sModelDatas_) {
		if (modelData.name == filename) {
			//メッシュの作成
			model->mesh_ = std::make_unique<Mesh>();
			model->mesh_->Initialize(modelData.vertices);

			//テクスチャの読み込み
			model->textureHandle_ = TextureManager::Load(modelData.material.textureFilePath);

			//マテリアルの作成
			model->material_ = std::make_unique<Material>();
			model->material_->Initialize();

			//DirectionalLightの作成
			model->directionalLight_ = std::make_unique<DirectionalLight>();
			model->directionalLight_->Initialize();

			return model;
		}
	}

	//モデルデータを読み込む
	ModelData modelData = model->LoadObjFile(directoryPath, filename);
	modelData.name = filename;
	sModelDatas_.push_back(modelData);

	//テクスチャを読み込む
	model->textureHandle_ = TextureManager::Load(modelData.material.textureFilePath);

	//メッシュの作成
	model->mesh_ = std::make_unique<Mesh>();
	model->mesh_->Initialize(modelData.vertices);

	//マテリアルの作成
	model->material_ = std::make_unique<Material>();
	model->material_->Initialize();

	model->directionalLight_ = std::make_unique<DirectionalLight>();
	model->directionalLight_->Initialize();

	return model;
}

void Model::Draw(const WorldTransform& worldTransform, const Camera& camera) {
	//マテリアルの更新
	material_->Update();
	//DirectionalLightの更新
	directionalLight_->Update();

	//GraphicsContextのインスタンスを取得
	GraphicsContext* graphicsContext = GraphicsContext::GetInstance();
	//頂点データを設定
	mesh_->SetGraphicsCommand();
	//マテリアルを設定
	material_->SetGraphicsCommand(UINT(RootParameterIndex::Material));
	//WorldTransformを設定
	graphicsContext->SetConstantBuffer(UINT(RootParameterIndex::WorldlTransform), worldTransform.GetConstantBuffer()->GetGPUVirtualAddress());
	//Cameraを設定
	graphicsContext->SetConstantBuffer(UINT(RootParameterIndex::Camera), camera.GetConstantBuffer()->GetGPUVirtualAddress());
	//DescriptorHeapを設定
	TextureManager::GetInstance()->SetGraphicsDescriptorHeap();
	//DescriptorTableを設定
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex::Texture), textureHandle_);
	//DirectionalLightを設定
	directionalLight_->SetGraphicsCommand(UINT(RootParameterIndex::DirectionalLight));
	//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	mesh_->Draw();
}

void Model::Draw(const WorldTransform& worldTransform, const Camera& camera, uint32_t textureHandle) {
	//マテリアルの更新
	material_->Update();
	//DirectionalLightの更新
	directionalLight_->Update();

	//GraphicsContextのインスタンスを取得
	GraphicsContext* graphicsContext = GraphicsContext::GetInstance();
	//頂点データを設定
	mesh_->SetGraphicsCommand();
	//マテリアルを設定
	material_->SetGraphicsCommand(UINT(RootParameterIndex::Material));
	//WorldTransformを設定
	graphicsContext->SetConstantBuffer(UINT(RootParameterIndex::WorldlTransform), worldTransform.GetConstantBuffer()->GetGPUVirtualAddress());
	//Cameraを設定
	graphicsContext->SetConstantBuffer(UINT(RootParameterIndex::Camera), camera.GetConstantBuffer()->GetGPUVirtualAddress());
	//DescriptorHeapを設定
	TextureManager::GetInstance()->SetGraphicsDescriptorHeap();
	//DescriptorTableを設定
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(UINT(RootParameterIndex::Texture), textureHandle);
	//DirectionalLightを設定
	directionalLight_->SetGraphicsCommand(UINT(RootParameterIndex::DirectionalLight));
	//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	mesh_->Draw();
}

Model::ModelData Model::LoadObjFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;//構築するModelData
	std::vector<Vector4> positions;//位置
	std::vector<Vector3> normals;//法線
	std::vector<Vector2> texcoords;//テクスチャ座標
	std::string line;//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;//先頭の識別子を読む

		//identifierに応じた処理
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f;
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord;
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.z *= -1.0f;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			Mesh::VertexData triangle[3];
			//面は三角形限定。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				//頂点の要素へのIndexは「位置/UV/法線」で格納されているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/');// /区切りでインデックスを読んでいく
					elementIndices[element] = std::stoi(index);
				}
				//要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				triangle[faceVertex] = { position,texcoord,normal };
			}
			//頂点を逆順で登録することで、回り順を逆にする
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		}
		else if (identifier == "mtllib") {
			//materialTempalteLibraryファイルの名前を取得する
			std::string materialFilename;
			s >> materialFilename;
			//基本的にobjファイルと同一階層にmtlは存在させるので、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}
	return modelData;
}

Model::MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;//構築するMaterialData
	std::string line;//ファイルから読んだ1行を格納するもの
	std::ifstream file(directoryPath + "/" + filename);//ファイルを開く
	assert(file.is_open());//とりあえず開けなかったら止める

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		//identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			//連結してファイルパスにする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}
	return materialData;
}