#include "TextureResource.h"
#include "Engine/Base/Graphics/GraphicsCommon.h"

void TextureResource::Create(const DirectX::TexMetadata& metadata) {
	//デバイスの取得
	device_ = GraphicsCommon::GetInstance()->GetDevice();

	//リソースの状態の初期化
	resourceState_ = D3D12_RESOURCE_STATE_COPY_DEST;

	//metadataを基にResourceの設定
	resourceDesc_.Width = UINT(metadata.width);//Textureの幅
	resourceDesc_.Height = UINT(metadata.height);//Textureの高さ
	resourceDesc_.MipLevels = UINT16(metadata.mipLevels);//mipmapの数
	resourceDesc_.DepthOrArraySize = UINT16(metadata.arraySize);//奥行or配列Textureの配列数
	resourceDesc_.Format = metadata.format;//TextureのFormat
	resourceDesc_.SampleDesc.Count = 1;//サンプルカウント。1固定
	resourceDesc_.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);//Textureの次元数。普段使っているのは2次元

	//利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//Resourceの作成
	HRESULT hr = device_->CreateCommittedResource(
		&heapProperties,//Heapの設定
		D3D12_HEAP_FLAG_NONE,//Heapの特殊な設定。特になし。
		&resourceDesc_,//Resourceの設定
		resourceState_,//データ転送される設定
		nullptr,//Clear最適値。使わないのでnullptr
		IID_PPV_ARGS(&resource_));//作成するResourceのポインタ
	assert(SUCCEEDED(hr));
}