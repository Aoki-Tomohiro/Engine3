#pragma once
#include "Engine/Base/Buffer/ColorBuffer.h"
#include "Engine/Base/Buffer/DepthBuffer.h"

class GraphicsContext {
public:
	/// <summary>
	/// シングルトンインスタンスを取得
	/// </summary>
	/// <returns></returns>
	static GraphicsContext* GetInstance();

	/// <summary>
	/// シングルトンインスタンスを削除
	/// </summary>
	static void DeleteInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="device"></param>
	void Initialize(ID3D12Device* device);

	/// <summary>
	/// リソースの状態遷移
	/// </summary>
	void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState);

	/// <summary>
	/// レンダーターゲットを設定
	/// </summary>
	/// <param name="count"></param>
	/// <param name="rtvHandles"></param>
	void SetRenderTargets(UINT count, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[]);

	/// <summary>
	/// レンダーターゲットを設定
	/// </summary>
	/// <param name="count"></param>
	/// <param name="rtvHandles"></param>
	/// <param name="dsvHandle"></param>
	void SetRenderTargets(UINT count, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[], D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle);

	/// <summary>
	/// レンダーターゲットをクリア
	/// </summary>
	/// <param name="resource"></param>
	void ClearRenderTarget(ColorBuffer& resource);

	/// <summary>
	/// 深度バッファをクリア
	/// </summary>
	/// <param name="depthBuffer"></param>
	void ClearDepthBuffer(DepthBuffer& depthBuffer);

	/// <summary>
	/// ビューポートを設定
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	/// <param name="topLeftX"></param>
	/// <param name="topLeftY"></param>
	/// <param name="minDepth"></param>
	/// <param name="maxDepth"></param>
	void SetViewport(FLOAT width, FLOAT height, FLOAT topLeftX, FLOAT topLeftY, FLOAT minDepth, FLOAT maxDepth);

	/// <summary>
	/// シザー矩形を設定
	/// </summary>
	/// <param name="left"></param>
	/// <param name="right"></param>
	/// <param name="top"></param>
	/// <param name="bottom"></param>
	void SetScissorRect(UINT left, UINT right, UINT top, UINT bottom);

	/// <summary>
	/// VertexBufferを設定
	/// </summary>
	/// <param name="vertexBufferView"></param>
	void SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView);

	/// <summary>
	/// 形状を設定
	/// </summary>
	/// <param name="primitiveTopology"></param>
	void SetPrimitiveTopology(const D3D_PRIMITIVE_TOPOLOGY& primitiveTopology);

	/// <summary>
	/// ConstantBufferを設定
	/// </summary>
	/// <param name="rootParameterIndex"></param>
	/// <param name="gpuAddress"></param>
	void SetConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress);

	/// <summary>
	/// DescriptorTableを設定
	/// </summary>
	/// <param name="rootParameterIndex"></param>
	/// <param name="gpuHandle"></param>
	void SetDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	/// <summary>
	/// DescriptorHeapを設定
	/// </summary>
	/// <param name="count"></param>
	/// <param name="descriptorHeap"></param>
	void SetDescriptorHeaps(UINT count, ID3D12DescriptorHeap* descriptorHeap[]);

	/// <summary>
	/// ルートシグネチャを設定
	/// </summary>
	/// <param name="rootSignature"></param>
	void SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature);

	/// <summary>
	/// PSOを設定
	/// </summary>
	/// <param name="pipelineState"></param>
	void SetPipelineState(const Microsoft::WRL::ComPtr<ID3D12PipelineState>& pipelineState);

	/// <summary>
	/// ドローコール
	/// </summary>
	/// <param name="vertexCount"></param>
	/// <param name="instanceCount"></param>
	void DrawInstanced(UINT vertexCount, UINT instanceCount);

	/// <summary>
	/// 実行
	/// </summary>
	void Execute();

	/// <summary>
	/// シグナルを送信
	/// </summary>
	void Signal();

	/// <summary>
	/// Fenceが指定した値に到達するまで待機
	/// </summary>
	void WaitForFence();

	/// <summary>
	/// リセット
	/// </summary>
	void Reset();

	/// <summary>
	/// コマンドリストを取得
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); };

	/// <summary>
	/// コマンドキューを取得
	/// </summary>
	/// <returns></returns>
	ID3D12CommandQueue* GetCommandQueue() const { return commandQueue_.Get(); };

private:
	GraphicsContext() = default;
	~GraphicsContext() = default;
	GraphicsContext(const GraphicsContext&) = delete;
	GraphicsContext& operator=(const GraphicsContext&) = delete;

private:
	//インスタンス
	static GraphicsContext* instance_;
	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_ = nullptr;
	//コマンドアロケーター
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_ = nullptr;
	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_ = nullptr;
	//フェンス
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
	uint64_t fenceValue_{};
};

