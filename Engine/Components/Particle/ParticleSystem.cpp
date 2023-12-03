#include "ParticleSystem.h"
#include "Engine/Base/GraphicsCommon/GraphicsCommon.h"
#include "Engine/Base/TextureManager/TextureManager.h"

//実体定義
ParticleSystem* ParticleSystem::instance_ = nullptr;
ID3D12Device* ParticleSystem::sDevice_ = nullptr;
ID3D12GraphicsCommandList* ParticleSystem::sCommandList_ = nullptr;

void ParticleSystem::StaticInitialize() {
	//コマンドリストの取得
	sCommandList_ = GraphicsCommon::GetInstance()->GetCommandList();
	sDevice_ = GraphicsCommon::GetInstance()->GetDevice();
}

void ParticleSystem::Initialize() {
	//頂点データの作成
	vertices_.push_back({ .position{1.0f, 1.0f, 0.0f, 1.0f }, .texcoord{ 0.0f, 0.0f }, .normal{ 0.0f, 0.0f, 1.0f } }); // 左上
	vertices_.push_back({ .position{-1.0f, 1.0f, 0.0f, 1.0f }, .texcoord{ 1.0f, 0.0f }, .normal{ 0.0f, 0.0f, 1.0f } }); // 左下
	vertices_.push_back({ .position{1.0f, -1.0f, 0.0f, 1.0f}, .texcoord{0.0f, 1.0f}, .normal{0.0f, 0.0f, 1.0f} }); // 右上
	vertices_.push_back({ .position{1.0f, -1.0f, 0.0f, 1.0f}, .texcoord{0.0f, 1.0f}, .normal{0.0f, 0.0f, 1.0f} }); // 左下
	vertices_.push_back({ .position{-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord{1.0f, 0.0f}, .normal{0.0f, 0.0f, 1.0f} }); // 右下
	vertices_.push_back({ .position{-1.0f, -1.0f, 0.0f, 1.0f}, .texcoord{1.0f, 1.0f}, .normal{0.0f, 0.0f, 1.0f} }); // 右上

	////頂点データの作成
	//vertices_.push_back({ .position{-1.0f, -1.0f, 0.0f, 1.0f }, .texcoord{ 0.0f, 0.0f }, .normal{ 0.0f, 0.0f, 1.0f } });
	//vertices_.push_back({ .position{-1.0f, 1.0f, 0.0f, 1.0f }, .texcoord{ 0.0f, 1.0f }, .normal{ 0.0f, 0.0f, 1.0f } });
	//vertices_.push_back({ .position{1.0f, -1.0f, 0.0f, 1.0f}, .texcoord{1.0f, 0.0f}, .normal{0.0f, 0.0f, 1.0f} });
	//vertices_.push_back({ .position{-1.0f, 1.0f, 0.0f, 1.0f}, .texcoord{0.0f, 1.0f}, .normal{0.0f, 0.0f, 1.0f} });
	//vertices_.push_back({ .position{1.0f, 1.0f, 0.0f, 1.0f}, .texcoord{1.0f, 1.0f}, .normal{0.0f, 0.0f, 1.0f} });
	//vertices_.push_back({ .position{1.0f, -1.0f, 0.0f, 1.0f}, .texcoord{1.0f, 0.0f}, .normal{0.0f, 0.0f, 1.0f} });

	//頂点バッファの作成
	CreateVertexBuffer();

	//マテリアル用のリソースの作成
	CreateMaterialResource();

	//インスタンシング用のリソースの作成
	CreateInstancingResource();
	//SRVの作成
	srvHandleGpu_ = TextureManager::GetInstance()->CreateInstancingShaderResourceView(*instancingResource_.get(), kMaxInstance, sizeof(ParticleForGPU));

	//テクスチャの読み込み
	textureHandle_ = TextureManager::Load("Resources/Images/circle.png");
}

void ParticleSystem::Update() {
	//エミッターの削除
	particleEmitters_.remove_if([](std::unique_ptr<ParticleEmitter>& particleEmitter) {
		if (particleEmitter->GetIsDead()) {
			particleEmitter.reset();
			return true;
		}
		return false;
		}
	);

	//エミッターの更新
	for (std::unique_ptr<ParticleEmitter>& emitter : particleEmitters_) {
		emitter->Update();
	}
}

void ParticleSystem::Draw(const Camera& camera) {
	//InstancingResourceの更新
	UpdateInstancingResource(camera);

	//DescriptorHeapを設定
	TextureManager::GetInstance()->SetGraphicsDescriptorHeap();
	//VBVを設定
	sCommandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//WorldTransform用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootDescriptorTable(1, srvHandleGpu_);
	//ViewProjection用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(2, camera.GetConstantBuffer()->GetGPUVirtualAddress());
	//DescriptorTableを設定
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(3, textureHandle_);
	//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	sCommandList_->DrawInstanced(UINT(vertices_.size()), numInstance_, 0, 0);
}

void ParticleSystem::CreateVertexBuffer() {
	//頂点リソースを作る
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sDevice_, sizeof(VertexData) * vertices_.size());

	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());
	//頂点1つあたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexData_ = static_cast<VertexData*>(vertexBuffer_->Map());
	std::memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexBuffer_->Unmap();
}

void ParticleSystem::CreateMaterialResource() {
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = std::make_unique<UploadBuffer>();
	materialResource_->Create(sDevice_,sizeof(MaterialData));
	//マテリアルにデータを書き込む
	materialData_ = static_cast<MaterialData*>(materialResource_->Map());
	//今回は赤を書き込んでみる
	materialData_->color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	materialData_->uvTransform = MakeIdentity4x4();
	materialResource_->Unmap();
}

void ParticleSystem::CreateInstancingResource() {
	//Instancing用のWorldTransformリソースを作る
	instancingResource_ = std::make_unique<UploadBuffer>();
	instancingResource_->Create(sDevice_,sizeof(ParticleForGPU) * kMaxInstance);
	//書き込むためのアドレスを取得
	instancingData_ = static_cast<ParticleForGPU*>(instancingResource_->Map());
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kMaxInstance; ++index) {
		instancingData_[index].world = MakeIdentity4x4();
		instancingData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	instancingResource_->Unmap();
}

void ParticleSystem::UpdateInstancingResource(const Camera& camera) {
	ParticleForGPU* instancingData = nullptr;
	numInstance_ = 0;
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 cameraMatrix = MakeAffineMatrix({ 1.0f,1.0f,1.0f }, camera.rotation_, camera.translation_);
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, cameraMatrix);
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	instancingData_ = static_cast<ParticleForGPU*>(instancingResource_->Map());
	for (std::list<std::unique_ptr<ParticleEmitter>>::iterator emitterIterator = particleEmitters_.begin(); emitterIterator != particleEmitters_.end();) {
		//パーティクルのリストを取得
		std::list<std::unique_ptr<Particle>>& particles = emitterIterator->get()->GetParticles();
		for (std::list<std::unique_ptr<Particle>>::iterator particleIterator = particles.begin(); particleIterator != particles.end();) {
			if (particleIterator->get()->IsDead()) {
				particleIterator = particles.erase(particleIterator);
				continue;
			}
			Matrix4x4 scaleMatrix = MakeScaleMatrix(particleIterator->get()->GetScale());
			Matrix4x4 translateMatrix = MakeTranslateMatrix(particleIterator->get()->GetTranslation());
			Matrix4x4 worldMatrix = Multiply(scaleMatrix, Multiply(billboardMatrix, translateMatrix));
			if (numInstance_ < kMaxInstance) {
				instancingData_[numInstance_].world = worldMatrix;
				instancingData_[numInstance_].color = particleIterator->get()->GetColor();
				numInstance_++;
			}
			++particleIterator;
		}
		++emitterIterator;
	}
	instancingResource_->Unmap();
}

ParticleEmitter* ParticleSystem::GetParticleEmitter(const std::string& name) {
	//エミッターのリストから探す
	for (std::unique_ptr<ParticleEmitter>& particleEmitter : particleEmitters_) {
		if (particleEmitter->GetName() == name) {
			return particleEmitter.get();
		}
	}
	//見つからなかったらnullptrを返す
	return nullptr;
}

std::list<ParticleEmitter*> ParticleSystem::GetParticleEmitters(const std::string& name) {
	//返却するリスト
	std::list<ParticleEmitter*> particleEmitters{};
	//エミッターをリストから探す
	for (std::unique_ptr<ParticleEmitter>& particleEmitter : particleEmitters_) {
		if (particleEmitter->GetName() == name) {
			particleEmitters.push_back(particleEmitter.get());
		}
	}
	return particleEmitters;
}