#pragma once
#include "Engine/Base/Graphics/GraphicsCommon.h"
#include "Engine/Base/RootSignature/RootSignature.h"

class PipelineState {
public:
	/// <summary>
	/// ルートシグネチャを設定
	/// </summary>
	/// <param name="rootSignature"></param>
	void SetRootSignature(const RootSignature* rootSignature);

	/// <summary>
	/// InputLayoutを設定
	/// </summary>
	/// <param name="NumElements"></param>
	/// <param name="pInputElementDescs"></param>
	void SetInputLayout(UINT count, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs);

	/// <summary>
	/// VertexShaderを設定
	/// </summary>
	/// <param name="Binary"></param>
	/// <param name="Size"></param>
	void SetVertexShader(const void* Binary, size_t Size);

	/// <summary>
	/// PixelShaderを設定
	/// </summary>
	/// <param name="Binary"></param>
	/// <param name="Size"></param>
	void SetPixelShader(const void* Binary, size_t Size);

	/// <summary>
	/// BlendStateを設定
	/// </summary>
	/// <param name="blendDesc"></param>
	void SetBlendState(const D3D12_BLEND_DESC& blendDesc);

	/// <summary>
	/// RasterizerDescを設定
	/// </summary>
	/// <param name="rasterizerDesc"></param>
	void SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc);

	/// <summary>
	/// RenderTargetを設定
	/// </summary>
	/// <param name="count"></param>
	/// <param name="rtvFormats"></param>
	void SetRenderTargetFormat(UINT count, const DXGI_FORMAT* rtvFormats);

	/// <summary>
	/// 形状を設定
	/// </summary>
	/// <param name="primitiveTopologyType"></param>
	void SetPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY_TYPE& primitiveTopologyType);

	/// <summary>
	/// サンプルデスクを設定
	/// </summary>
	/// <param name="count"></param>
	void SetSampleDesc(UINT count);

	/// <summary>
	/// DepthStencilStateを設定
	/// </summary>
	/// <param name="depthStencilState"></param>
	void SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc);

	/// <summary>
	/// DepthStencilFormatを設定
	/// </summary>
	/// <param name="format"></param>
	void SetDepthStencilFormat(const DXGI_FORMAT& format);

	/// <summary>
	/// 作成
	/// </summary>
	void Finalize();

	/// <summary>
	/// PSOを取得
	/// </summary>
	/// <returns></returns>
	ID3D12PipelineState* GetPipelineState() const { return pipelineState_.Get(); };

private:
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_ = nullptr;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc_{};
};

