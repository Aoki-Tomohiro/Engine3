#include "TextureManager.h"
#include "Engine/Base/GraphicsCommon/GraphicsCommon.h"
#include "Engine/Utilities/Log.h"
#include <cassert>

//実体定義
TextureManager* TextureManager::instance = nullptr;

TextureManager* TextureManager::GetInstance() {
	if (instance == nullptr) {
		instance = new TextureManager();
	}
	return instance;
}

void TextureManager::DeleteInstnace() {
	delete instance;
	instance = nullptr;
}

uint32_t TextureManager::Load(const std::string& filePath) {
	//テクスチャを読み込む
	uint32_t textureHandle = TextureManager::GetInstance()->LoadInternal(filePath);

	return textureHandle;
}

void TextureManager::Initialize() {
	//デバイスの取得
	device_ = GraphicsCommon::GetInstance()->GetDevice();
	//コマンドリストの取得
	commandList_ = GraphicsCommon::GetInstance()->GetCommandList();

	//ディスクリプタヒープの作成
	srvDescriptorHeap_ = std::make_unique<SRVHeap>();
	srvDescriptorHeap_->Create(device_, kNumDescriptors);

	//デフォルト画像を読み込む
	LoadInternal("Resources/Images/white.png");
}

void TextureManager::SetGraphicsDescriptorHeap() {
	//DescriptorHeapを設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_->GetDescriptorHeap() };
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);
}

void TextureManager::SetGraphicsRootDescriptorTable(UINT rootParameterIndex, uint32_t textureHandle) {
	//SRVのDescriptorTableの先頭を設定
	commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, textures_[textureHandle].resource->GetSRVGpuHandle());
}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::CreateInstancingShaderResourceView(UploadBuffer& instancingResource, uint32_t kNumInstance, size_t size) {
	//テクスチャハンドルをインクリメント
	textureHandle_++;
	//テクスチャがディスクリプタの最大数を超えていたら止める
	if (textureHandle_ >= kNumDescriptors) {
		assert(0);
	}
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGpu = srvDescriptorHeap_->CreateInstancingShaderResourceView(instancingResource, kNumInstance, size);
	textures_[textureHandle_].name = "instancingResource";
	return srvHandleGpu;
}

const D3D12_RESOURCE_DESC TextureManager::GetResourceDesc(uint32_t textureHandle) {
	//テクスチャの情報を取得
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc = textures_[textureHandle].resource->GetResourceDesc();

	return resourceDesc;
}

uint32_t TextureManager::LoadInternal(const std::string& filePath) {
	//同じテクスチャがないか探す
	for (Texture& texture : textures_) {
		if (texture.name == filePath) {
			return texture.textureHandle;
		}
	}

	//テクスチャハンドルをインクリメント
	textureHandle_++;
	//テクスチャがディスクリプタの最大数を超えていたら止める
	if (textureHandle_ >= kNumDescriptors) {
		assert(0);
	}

	//テクスチャを読み込む
	DirectX::ScratchImage mipImages = LoadTexture(filePath);

	//メタデータを取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//テクスチャ情報を基にリソースを作成
	textures_[textureHandle_].resource = std::make_unique<TextureResource>();
	textures_[textureHandle_].resource->Create(device_, metadata);
	//テクスチャのリソースにデータを転送する
	textures_[textureHandle_].intermediateResource = UploadTextureData(textures_[textureHandle_].resource.get(), mipImages);

	//SRVの作成
	srvDescriptorHeap_->CreateShaderResourceView(*textures_[textureHandle_].resource, metadata.format, UINT(metadata.mipLevels));

	//テクスチャの名前を保存する
	textures_[textureHandle_].name = filePath;
	textures_[textureHandle_].textureHandle = textureHandle_;

	return textureHandle_;
}

DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
	//テクスチャファイルを読んでプログラムで扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	//ミップマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));

	//ミップマップ付きのデータを返す
	return mipImages;
}

[[nodiscard]]
std::unique_ptr<UploadBuffer> TextureManager::UploadTextureData(TextureResource* texture, const DirectX::ScratchImage& mipImages) {
	//中間リソースを作成
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;
	DirectX::PrepareUpload(device_, mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresources);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture->GetResource(), 0, UINT(subresources.size()));
	std::unique_ptr<UploadBuffer> intermediateResource = std::make_unique<UploadBuffer>();
	intermediateResource->Create(device_, intermediateSize);

	//データ転送をコマンドに積む
	UpdateSubresources(commandList_, texture->GetResource(), intermediateResource->GetResource(), 0, 0, UINT(subresources.size()), subresources.data());

	//Textureへの転送後は利用できるよう、D3D12_RESOURCE_STATE_COPY_DESTからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	GraphicsCommon::GetInstance()->TransitionResource(*texture, D3D12_RESOURCE_STATE_GENERIC_READ);

	return intermediateResource;
}