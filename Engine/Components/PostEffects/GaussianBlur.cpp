#include "GaussianBlur.h"

void GaussianBlur::Initialize() {
	//GraphicsCommonのインスタンスを取得
	graphicsCommon_ = GraphicsCommon::GetInstance();

	//レンダラーのインスタンスを取得する
	renderer_ = Renderer::GetInstance();

	//ブラー用のCBVを作成
	CreateBlurUploadBuffer();

	//縮小ブラー用のCBVを作成
	CreateShrinkBlurUploadBuffer();

	//PSOの作成
	CreatePipelineState();
}

void GaussianBlur::CreateBlurUploadBuffer() {
	//ブラー用のCBVの作成
	blurUploadBuffer_ = std::make_unique<UploadBuffer>();
	blurUploadBuffer_->Create(sizeof(BlurData));
	blurData_ = static_cast<BlurData*>(blurUploadBuffer_->Map());
	blurData_->textureWidth = Application::GetInstance()->kClientWidth;
	blurData_->textureHeight = Application::GetInstance()->kClientHeight;
	float total = 0.0f;
	for (int i = 0; i < 8; i++) {
		blurData_->weight[i] = expf(-(i * i) / (2 * 5.0f * 5.0f));
		total += blurData_->weight[i];
	}
	total = total * 2.0f - 1.0f;
	//最終的な合計値で重みをわる
	for (int i = 0; i < 8; i++) {
		blurData_->weight[i] /= total;
	}
	blurUploadBuffer_->Unmap();
}

void GaussianBlur::CreateShrinkBlurUploadBuffer() {
	//ブラー用のCBVの作成
	shrinkBlurUploadBuffer_ = std::make_unique<UploadBuffer>();
	shrinkBlurUploadBuffer_->Create(sizeof(BlurData));
	shrinkBlurData_ = static_cast<BlurData*>(shrinkBlurUploadBuffer_->Map());
	shrinkBlurData_->textureWidth = Application::GetInstance()->kClientWidth;
	shrinkBlurData_->textureHeight = Application::GetInstance()->kClientHeight;
	float total = 0.0f;
	for (int i = 0; i < 8; i++) {
		shrinkBlurData_->weight[i] = expf(-(i * i) / (2 * 5.0f * 5.0f));
		total += shrinkBlurData_->weight[i];
	}
	total = total * 2.0f - 1.0f;
	//最終的な合計値で重みをわる
	for (int i = 0; i < 8; i++) {
		shrinkBlurData_->weight[i] /= total;
	}
	shrinkBlurUploadBuffer_->Unmap();
}

void GaussianBlur::CreatePipelineState() {
	//生成
	rootSignature_ = std::make_unique<RootSignature>();

	//DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算
	descriptorRange[1].BaseShaderRegister = 1;//0から始まる
	descriptorRange[1].NumDescriptors = 1;//数は1つ
	descriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。今回は結果一つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = &descriptorRange[0];//Tableの中身の配列を指定
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootSignature_->AddRootParameter(rootParameters[0]);
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = &descriptorRange[1];//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = 1;//Tableで利用する数
	rootSignature_->AddRootParameter(rootParameters[1]);
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[2].Descriptor.ShaderRegister = 0;
	rootSignature_->AddRootParameter(rootParameters[2]);


	//Sampler作成
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootSignature_->AddStaticSampler(staticSamplers[0]);
	//バイナリを元に生成
	rootSignature_->Finalize();


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//シェーダーをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob[2];
	vertexShaderBlob[0] = renderer_->CompileShader(L"HorizontalBlurVS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob[0] != nullptr);
	vertexShaderBlob[1] = renderer_->CompileShader(L"VerticalBlurVS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob[1] != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob[2];
	pixelShaderBlob[0] = renderer_->CompileShader(L"HorizontalBlurPS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob[0] != nullptr);
	pixelShaderBlob[1] = renderer_->CompileShader(L"VerticalBlurPS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob[1] != nullptr);

	
	//RTVのフォーマットを設定
	DXGI_FORMAT rtvFormats[2]{};
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;


	for (uint32_t i = 0; i < kCountOfBlur; i++) {
		pipelineStates_.push_back(std::make_unique<PipelineState>());
		pipelineStates_[i]->SetRootSignature(rootSignature_.get());
		pipelineStates_[i]->SetInputLayout(2, inputElementDescs);
		pipelineStates_[i]->SetVertexShader(vertexShaderBlob[i]->GetBufferPointer(), vertexShaderBlob[i]->GetBufferSize());
		pipelineStates_[i]->SetPixelShader(pixelShaderBlob[i]->GetBufferPointer(), pixelShaderBlob[i]->GetBufferSize());
		pipelineStates_[i]->SetBlendState(blendDesc);
		pipelineStates_[i]->SetRasterizerState(rasterizerDesc);
		pipelineStates_[i]->SetRenderTargetFormat(2, rtvFormats);
		pipelineStates_[i]->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipelineStates_[i]->SetSampleDesc(1);
		pipelineStates_[i]->Finalize();
	}
}