#include "PipelineState.h"

void PipelineState::SetRootSignature(const RootSignature* rootSignature) {
	pipelineStateDesc_.pRootSignature = rootSignature->GetRootSignature();
}

void PipelineState::SetInputLayout(UINT count, const D3D12_INPUT_ELEMENT_DESC* inputElementDescs) {
	pipelineStateDesc_.InputLayout.pInputElementDescs = inputElementDescs;
	pipelineStateDesc_.InputLayout.NumElements = count;
}

void PipelineState::SetVertexShader(const void* Binary, size_t Size) {
	pipelineStateDesc_.VS = { Binary,Size };
}

void PipelineState::SetPixelShader(const void* Binary, size_t Size) {
	pipelineStateDesc_.PS = { Binary,Size };
}

void PipelineState::SetBlendState(const D3D12_BLEND_DESC& blendDesc) {
	pipelineStateDesc_.BlendState = blendDesc;
}

void PipelineState::SetRasterizerState(const D3D12_RASTERIZER_DESC& rasterizerDesc) {
	pipelineStateDesc_.RasterizerState = rasterizerDesc;
}

void PipelineState::SetRenderTargetFormat(UINT count, const DXGI_FORMAT* rtvFormats) {
	pipelineStateDesc_.NumRenderTargets = count;
	for (uint32_t i = 0; i < pipelineStateDesc_.NumRenderTargets; i++) {
		pipelineStateDesc_.RTVFormats[i] = rtvFormats[i];
	}
}

void PipelineState::SetPrimitiveTopology(const D3D12_PRIMITIVE_TOPOLOGY_TYPE& primitiveTopologyType) {
	pipelineStateDesc_.PrimitiveTopologyType = primitiveTopologyType;
}

void PipelineState::SetSampleDesc(UINT count) {
	pipelineStateDesc_.SampleDesc.Count = count;
	pipelineStateDesc_.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
}

void PipelineState::SetDepthStencilState(const D3D12_DEPTH_STENCIL_DESC& depthStencilDesc) {
	pipelineStateDesc_.DepthStencilState = depthStencilDesc;
}

void PipelineState::SetDepthStencilFormat(const DXGI_FORMAT& format) {
	pipelineStateDesc_.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
}

void PipelineState::Finalize() {
	ID3D12Device* device = GraphicsCommon::GetInstance()->GetDevice();
	HRESULT hr = device->CreateGraphicsPipelineState(&pipelineStateDesc_, IID_PPV_ARGS(&pipelineState_));
	assert(SUCCEEDED(hr));
}