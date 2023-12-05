#include "Mesh.h"

void Mesh::Initialize(const std::vector<VertexData>& vertices) {
	//頂点情報を取得
	vertices_ = vertices;
	//頂点バッファを作成
	ID3D12Device* device = GraphicsCommon::GetInstance()->GetDevice();
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
	//コマンドリストを取得
	ID3D12GraphicsCommandList* commandList = GraphicsCommon::GetInstance()->GetCommandList();
	//VBVを設定
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	//形状を設定。PSOに設定しているものとは別。同じものを設定すると考えておけば良い
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Mesh::Draw() {
	//コマンドリストを取得
	ID3D12GraphicsCommandList* commandList = GraphicsCommon::GetInstance()->GetCommandList();
	//描画!(DrawCall/ドローコール)。3頂点で1つのインスタンス。インスタンスについては今後
	commandList->DrawInstanced(UINT(vertices_.size()), 1, 0, 0);
}