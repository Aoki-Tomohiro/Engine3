#pragma once
#include "Engine/Base/Buffer/UploadBuffer.h"
#include "Engine/Math/Structs/Vector2.h"
#include "Engine/Math/Structs/Vector3.h"
#include "Engine/Math/Structs/Vector4.h"
#include <vector>

class Mesh {
public:
	/// <summary>
	/// 頂点データ
	/// </summary>
	struct VertexData {
		Vector4 position;
		Vector2 texcoord;
		Vector3 normal;
	};

	/// <summary>
	/// メッシュの初期化
	/// </summary>
	/// <param name="vertices"></param>
	void Initialize(const std::vector<VertexData>& vertices);

	/// <summary>
	/// グラフィックスコマンドのセット
	/// </summary>
	void SetGraphicsCommand();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

private:
	//VertexBuffer
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;
	//書き込み用
	VertexData* vertexData_ = nullptr;
	//VertexBufferView
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	//頂点データ
	std::vector<VertexData> vertices_{};
};