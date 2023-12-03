#include "Sprite.h"

//実体定義
ID3D12Device* Sprite::sDevice_ = nullptr;
ID3D12GraphicsCommandList* Sprite::sCommandList_ = nullptr;
Matrix4x4 Sprite::sMatProjection_{};

void Sprite::StaticInitialize() {
	//デバイスの取得
	sDevice_ = GraphicsCore::GetInstance()->GetDevice();
	//コマンドリストの取得
	sCommandList_ = GraphicsCore::GetInstance()->GetCommandList();
	//平行投影行列の作成
	sMatProjection_ = MakeOrthographicMatrix(0.0f, 0.0f, 1280.0f, 720.0f, 0.0f, 100.0f);
}

Sprite* Sprite::Create(uint32_t textureHandle, Vector2 position) {
	//スプライトを作成
	Sprite* sprite = new Sprite();
	sprite->Initialize(textureHandle, position);
	return sprite;
}

void Sprite::Draw() {
	//非表示
	if (isInvisible_) {
		return;
	}

	////頂点バッファの更新
	UpdateVertexBuffer();
	//マテリアルの更新
	UpdateMaterial();
	//行列の更新
	UpdateMatrix();

	//VBVを設定
	sCommandList_->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
	sCommandList_->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//マテリアルCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());
	//wvp用のCBufferの場所を設定
	sCommandList_->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());
	//DescriptorHeapを設定
	TextureManager::GetInstance()->SetGraphicsDescriptorHeap();
	//DescriptorTableを設定
	TextureManager::GetInstance()->SetGraphicsRootDescriptorTable(2, textureHandle_);
	//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	sCommandList_->DrawInstanced(6, 1, 0, 0);
}

void Sprite::Initialize(uint32_t textureHandle, Vector2 position) {
	//座標を設定
	position_ = position;

	//テクスチャハンドルの設定
	textureHandle_ = textureHandle;
	//テクスチャの情報を取得
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
	//テクスチャサイズの初期化
	textureSize_ = { float(resourceDesc_.Width),float(resourceDesc_.Height) };
	size_ = { float(resourceDesc_.Width),float(resourceDesc_.Height) };;

	//頂点データの初期化
	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	float texLeft = textureLeftTop_.x / resourceDesc_.Width;
	float texRight = (textureLeftTop_.x + textureSize_.x) / resourceDesc_.Width;
	float texTop = textureLeftTop_.y / resourceDesc_.Height;
	float texBottom = (textureLeftTop_.y + textureSize_.y) / resourceDesc_.Height;

	//左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	vertices_[0].position = { left,bottom,0.0f,1.0f };
	vertices_[0].texcoord = { texLeft,texBottom };
	vertices_[1].position = { left,top,0.0f,1.0f };
	vertices_[1].texcoord = { texLeft,texTop };
	vertices_[2].position = { right,bottom,0.0f,1.0f };
	vertices_[2].texcoord = { texRight,texBottom };
	vertices_[3].position = { left,top,0.0f,1.0f };
	vertices_[3].texcoord = { texLeft,texTop };
	vertices_[4].position = { right,top,0.0f,1.0f };
	vertices_[4].texcoord = { texRight,texTop };
	vertices_[5].position = { right,bottom,0.0f,1.0f };
	vertices_[5].texcoord = { texRight,texBottom };

	//頂点バッファの作成
	CreateVertexBuffer();

	//マテリアル用のリソースを作成
	CreateMaterialResource();

	//WVP用のリソースを作成
	CreateWVPResource();
}

void Sprite::CreateVertexBuffer() {
	//頂点リソースを作る
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sDevice_, sizeof(VertexData) * 6);
	//リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点6つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 6;
	//頂点1つあたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//書き込むためのアドレスを取得
	vertexData_ = static_cast<VertexData*>(vertexBuffer_->Map());
	std::copy(std::begin(vertices_), std::end(vertices_), vertexData_);
	vertexBuffer_->Unmap();
}

void Sprite::UpdateVertexBuffer() {
	//テクスチャのサイズを合わせる
	AdjustTextureSize();
	//頂点データの更新
	float left = (0.0f - anchorPoint_.x) * size_.x;
	float right = (1.0f - anchorPoint_.x) * size_.x;
	float top = (0.0f - anchorPoint_.y) * size_.y;
	float bottom = (1.0f - anchorPoint_.y) * size_.y;
	float texLeft = textureLeftTop_.x / resourceDesc_.Width;
	float texRight = (textureLeftTop_.x + textureSize_.x) / resourceDesc_.Width;
	float texTop = textureLeftTop_.y / resourceDesc_.Height;
	float texBottom = (textureLeftTop_.y + textureSize_.y) / resourceDesc_.Height;
	//左右反転
	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}
	vertices_[0].position = { left,bottom,0.0f,1.0f };
	vertices_[0].texcoord = { texLeft,texBottom };
	vertices_[1].position = { left,top,0.0f,1.0f };
	vertices_[1].texcoord = { texLeft,texTop };
	vertices_[2].position = { right,bottom,0.0f,1.0f };
	vertices_[2].texcoord = { texRight,texBottom };
	vertices_[3].position = { left,top,0.0f,1.0f };
	vertices_[3].texcoord = { texLeft,texTop };
	vertices_[4].position = { right,top,0.0f,1.0f };
	vertices_[4].texcoord = { texRight,texTop };
	vertices_[5].position = { right,bottom,0.0f,1.0f };
	vertices_[5].texcoord = { texRight,texBottom };

	//書き込み
	vertexData_ = static_cast<VertexData*>(vertexBuffer_->Map());
	std::copy(std::begin(vertices_), std::end(vertices_), vertexData_);
	vertexBuffer_->Unmap();
}

void Sprite::CreateMaterialResource() {
	//マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResource_ = std::make_unique<UploadBuffer>();
	materialResource_->Create(sDevice_,sizeof(MaterialData));
	//マテリアルにデータを書き込む
	materialData_ = static_cast<MaterialData*>(materialResource_->Map());
	//今回は赤を書き込んでみる
	materialData_->color = color_;
	materialData_->uvTransform = MakeIdentity4x4();
	materialResource_->Unmap();
}

void Sprite::UpdateMaterial() {
	//書き込むためのアドレスを取得
	materialData_ = static_cast<MaterialData*>(materialResource_->Map());
	//今回は赤を書き込んでみる
	materialData_->color = color_;
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{ uvScale_.x,uvScale_.y,1.0f });
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(uvRotation_));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(Vector3{ uvTranslation_.x,uvTranslation_.y,0.0f }));
	materialData_->uvTransform = uvTransformMatrix;
	materialResource_->Unmap();
}

void Sprite::CreateWVPResource() {
	//WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResource_ = std::make_unique<UploadBuffer>();
	wvpResource_->Create(sDevice_, sizeof(Matrix4x4));
	//データを書き込む
	wvpData_ = static_cast<Matrix4x4*>(wvpResource_->Map());
	//単位行列を書き込んでおく
	*wvpData_ = MakeIdentity4x4();
	wvpResource_->Unmap();
}

void Sprite::UpdateMatrix() {
	//ワールド行列の作成
	Matrix4x4 worldMatrix = MakeAffineMatrix(Vector3(scale_.x, scale_.y, 1.0f), Vector3(0.0f, 0.0f, rotation_), Vector3(position_.x, position_.y, 0.0f));
	//ビュー行列の作成
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	//WVPMatrixの作成
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, sMatProjection_));

	//データを書き込む
	wvpData_ = static_cast<Matrix4x4*>(wvpResource_->Map());
	*wvpData_ = worldViewProjectionMatrix;
	wvpResource_->Unmap();
}

void Sprite::AdjustTextureSize() {
	//テクスチャの情報を取得
	resourceDesc_ = TextureManager::GetInstance()->GetResourceDesc(textureHandle_);
	//テクスチャサイズの初期化
	textureSize_ = { float(resourceDesc_.Width),float(resourceDesc_.Height) };
}