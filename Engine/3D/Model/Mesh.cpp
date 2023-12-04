#include "Mesh.h"
#include "Engine/Base/Graphics/GraphicsContext.h"

void Mesh::Initialize(const std::vector<VertexData>& vertices) {
	//頂点情報を取得
	vertices_ = vertices;
	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexData) * vertices_.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();//リソースの先頭のアドレスから使う
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexData) * vertices_.size());//使用するリソースのサイズは頂点のサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	//頂点バッファにデータを書き込む
	vertexData_ = static_cast<VertexData*>(vertexBuffer_->Map());
	std::memcpy(vertexData_, vertices_.data(), sizeof(VertexData) * vertices_.size());
	vertexBuffer_->Unmap();
}

void Mesh::SetGraphicsCommand() {
	//GraphicsContextのインスタンスを取得
	GraphicsContext* graphicsContext = GraphicsContext::GetInstance();
	//VBVを設定
	graphicsContext->SetVertexBuffer(vertexBufferView_);
	//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
	graphicsContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Draw() {
	//GraphicsContextのインスタンスを取得
	GraphicsContext* graphicsContext = GraphicsContext::GetInstance();
	//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	graphicsContext->DrawInstanced(UINT(vertices_.size()), 1);
}