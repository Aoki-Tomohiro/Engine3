#pragma once
#include <d3d12.h>
#include <wrl.h>
#include <cstdint>

class GpuResource {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~GpuResource() = default;

	/// <summary>
	/// リソースを習得
	/// </summary>
	/// <returns></returns>
	ID3D12Resource* GetResource() const { return resource_.Get(); };

	/// <summary>
	/// リソースステートを取得
	/// </summary>
	/// <returns></returns>
	const D3D12_RESOURCE_STATES& GetResourceState() { return resourceState_; };

	/// <summary>
	/// リソースステートを設定
	/// </summary>
	/// <param name="resourceState"></param>
	void SetResourceState(const D3D12_RESOURCE_STATES& resourceState) { resourceState_ = resourceState; };

protected:
	//デバイス
	ID3D12Device* device_ = nullptr;

	//リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

	//リソースの状態
	D3D12_RESOURCE_STATES resourceState_{};
};

