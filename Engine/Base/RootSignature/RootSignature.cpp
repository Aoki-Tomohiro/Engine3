#include "RootSignature.h"

void RootSignature::AddRootParameter(const D3D12_ROOT_PARAMETER& rootParameter) {
    rootParameters_.push_back(rootParameter);
}

void RootSignature::AddStaticSampler(const D3D12_STATIC_SAMPLER_DESC& staticSamplers) {
    staticSamplers_.push_back(staticSamplers);
}

void RootSignature::Finalize() {
    //RootSignature の作成
    D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature = {};
    descriptionRootSignature.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

    descriptionRootSignature.pParameters = rootParameters_.data();
    descriptionRootSignature.NumParameters = static_cast<UINT>(rootParameters_.size());

    // Static Sampler の設定
    descriptionRootSignature.pStaticSamplers = staticSamplers_.data();
    descriptionRootSignature.NumStaticSamplers = static_cast<UINT>(staticSamplers_.size());

    Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
        D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
        assert(false);
    }
    //バイナリを元に生成
    ID3D12Device* device = GraphicsCommon::GetInstance()->GetDevice();
    hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}