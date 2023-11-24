#include "Material.h"

void Material::Initialize() {
	//マテリアルリソースの作成
	materialResource_ = std::make_unique<UploadBuffer>();
	materialResource_->Create(sizeof(ConstBufferDataMaterial));

	//マテリアルリソースに書き込む
	materialData_ = static_cast<ConstBufferDataMaterial*>(materialResource_->Map());
	materialData_->color = color_;
	materialData_->uvTransform = MakeIdentity4x4();
	materialResource_->Unmap();
}

void Material::Update() {
	//行列の作成
	Matrix4x4 uvTransformMatrix = MakeScaleMatrix(Vector3{ scale_.x,scale_.y,1.0f });
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeRotateZMatrix(rotation_));
	uvTransformMatrix = Multiply(uvTransformMatrix, MakeTranslateMatrix(Vector3{ translation_.x,translation_.y,0.0f }));

	//マテリアルリソースに書き込む
	materialData_ = static_cast<ConstBufferDataMaterial*>(materialResource_->Map());
	materialData_->color = color_;
	materialData_->uvTransform = MakeIdentity4x4();
	materialResource_->Unmap();
}

void Material::SetGraphicsCommand(UINT rootParameterIndex) {
	//コマンドリストを取得
	ID3D12GraphicsCommandList* commandList = GraphicsCore::GetInstance()->GetCommandList();
	//マテリアルリソースを設定
	commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, materialResource_->GetGPUVirtualAddress());
}