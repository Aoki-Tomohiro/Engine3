#include "Renderer.h"

//実体定義
Renderer* Renderer::instance_ = nullptr;

Renderer* Renderer::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new Renderer();
	}
	return instance_;
}

void Renderer::DeleteInstance() {
	delete instance_;
	instance_ = nullptr;
}

void Renderer::Initialize() {
	//デバイスの取得
	graphicsCommon_ = GraphicsCommon::GetInstance();

	//DescriptorHeapの作成
	ID3D12Device* device = graphicsCommon_->GetDevice();
	rtvHeap_ = std::make_unique<RTVHeap>();
	rtvHeap_->Create(10);
	srvHeap_ = std::make_unique<SRVHeap>();
	srvHeap_->Create(10);
	dsvHeap_ = std::make_unique<DSVHeap>();
	dsvHeap_->Create(1);

	//リソースの作成
	float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
	sceneColorBuffer_ = std::make_unique<ColorBuffer>();
	sceneColorBuffer_->Create(Application::kClientWidth, Application::kClientHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, clearColor);
	sceneDepthBuffer_ = std::make_unique<DepthBuffer>();
	sceneDepthBuffer_->Create(Application::GetInstance()->kClientWidth, Application::GetInstance()->kClientHeight, DXGI_FORMAT_D24_UNORM_S8_UINT);

	//RTVの作成
	rtvHeap_->CreateRenderTargetView(*sceneColorBuffer_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	//SRVの作成
	srvHeap_->CreateShaderResourceView(*sceneColorBuffer_, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	//DSVの作成
	dsvHeap_->CreateDepthStencilView(*sceneDepthBuffer_, DXGI_FORMAT_D24_UNORM_S8_UINT);

	//シェーダーコンパイラーの初期化
	shaderCompiler_ = std::make_unique<ShaderCompiler>();
	shaderCompiler_->Initialize();

	//モデル用のPSOの作成
	CreateModelPipelineState();

	//スプライト用のPSOの作成
	CreateSpritePipelineState();

	//パーティクル用のPSOの作成
	CreateParticlePipelineState();
}

void Renderer::PreDraw() {
	//コマンドリストを取得
	ID3D12GraphicsCommandList* commandList = graphicsCommon_->GetCommandList();
	//リソースバリアを張る
	graphicsCommon_->TransitionResource(*sceneColorBuffer_, D3D12_RESOURCE_STATE_RENDER_TARGET);
	//レンダーターゲットを設定
	commandList->OMSetRenderTargets(1, &sceneColorBuffer_->GetRTVHandle(), false, &sceneDepthBuffer_->GetDSVHandle());
	//レンダーターゲットをクリア
	commandList->ClearRenderTargetView(sceneColorBuffer_->GetRTVHandle(), sceneColorBuffer_->GetClearColor(), 0, nullptr);

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = FLOAT(Application::GetInstance()->kClientWidth);
	viewport.Height = FLOAT(Application::GetInstance()->kClientHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポートを設定
	commandList->RSSetViewports(1, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = Application::GetInstance()->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = Application::GetInstance()->kClientHeight;
	//シザーを設定
	commandList->RSSetScissorRects(1, &scissorRect);
}

void Renderer::PostDraw() {
	//リソースバリアを張る
	graphicsCommon_->TransitionResource(*sceneColorBuffer_, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

void Renderer::Draw() {

}

void Renderer::PreDrawModels(ModelRenderingType type) {
	//コマンドリストを取得
	ID3D12GraphicsCommandList* commandList = graphicsCommon_->GetCommandList();
	//RootSignatureを設定
	commandList->SetGraphicsRootSignature(modelRootSignature_->GetRootSignature());
	//PipelineStateを設定
	commandList->SetPipelineState(modelPipelineStates_[type]->GetPipelineState());
}

void Renderer::PostDrawModels() {

}

void Renderer::PreDrawSprites(BlendMode blendMode) {
	//コマンドリストを取得
	ID3D12GraphicsCommandList* commandList = graphicsCommon_->GetCommandList();
	//RootSignatureを設定
	commandList->SetGraphicsRootSignature(spriteRootSignature_->GetRootSignature());
	//PipelineStateを設定
	commandList->SetPipelineState(spritePipelineStates_[blendMode]->GetPipelineState());
}

void Renderer::PostDrawSprites() {

}

void Renderer::PreDrawParticles() {
	//コマンドリストを取得
	ID3D12GraphicsCommandList* commandList = graphicsCommon_->GetCommandList();
	//RootSignatureを設定
	commandList->SetGraphicsRootSignature(particleRootSignature_->GetRootSignature());
	//PipelineStateを設定
	commandList->SetPipelineState(particlePipelineStates_[0]->GetPipelineState());
}

void Renderer::PostDrawParticles() {

}

void Renderer::CreateModelPipelineState() {
	//生成
	modelRootSignature_ = std::make_unique<RootSignature>();

	//DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。今回は結果一つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[5] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	modelRootSignature_->AddRootParameter(rootParameters[0]);
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
	modelRootSignature_->AddRootParameter(rootParameters[1]);
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
	modelRootSignature_->AddRootParameter(rootParameters[2]);
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	modelRootSignature_->AddRootParameter(rootParameters[3]);
	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[4].Descriptor.ShaderRegister = 1;//レジスタ番号１を使う
	modelRootSignature_->AddRootParameter(rootParameters[4]);

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
	modelRootSignature_->AddStaticSampler(staticSamplers[0]);
	//バイナリを元に生成
	modelRootSignature_->Finalize();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3]{};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = shaderCompiler_->CompileShader(L"Object3d.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = shaderCompiler_->CompileShader(L"Object3d.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc[2]{};
	//すべての色要素を書き込む
	blendDesc[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//透明オブジェクトのBlendStateの設定
	blendDesc[1].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blendDesc[1].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[1].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[1].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//半透明合成
	blendDesc[1].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc[1].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//書き込むRTVの情報
	DXGI_FORMAT rtvFormats[2]{};
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvFormats[1] = DXGI_FORMAT_R32_FLOAT;


	//PSOを作成する
	for (uint32_t i = 0; i < RenderingTypeCount; i++) {
		modelPipelineStates_.push_back(std::make_unique<PipelineState>());
		modelPipelineStates_[i]->SetRootSignature(modelRootSignature_.get());
		modelPipelineStates_[i]->SetInputLayout(3, inputElementDescs);
		modelPipelineStates_[i]->SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
		modelPipelineStates_[i]->SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
		modelPipelineStates_[i]->SetBlendState(blendDesc[i]);
		modelPipelineStates_[i]->SetRasterizerState(rasterizerDesc);
		modelPipelineStates_[i]->SetRenderTargetFormat(2, rtvFormats);
		modelPipelineStates_[i]->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		modelPipelineStates_[i]->SetSampleDesc(1);
		modelPipelineStates_[i]->SetDepthStencilState(depthStencilDesc);
		modelPipelineStates_[i]->SetDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);
		modelPipelineStates_[i]->Finalize();
	}
}

void Renderer::CreateSpritePipelineState() {
	//生成
	spriteRootSignature_ = std::make_unique<RootSignature>();

	//DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	//RootParameter作成。複数設定できるので配列。今回は結果一つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[3] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	spriteRootSignature_->AddRootParameter(rootParameters[0]);
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0;//レジスタ番号0を使う
	spriteRootSignature_->AddRootParameter(rootParameters[1]);
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	spriteRootSignature_->AddRootParameter(rootParameters[2]);

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
	spriteRootSignature_->AddStaticSampler(staticSamplers[0]);
	//バイナリを元に生成
	spriteRootSignature_->Finalize();


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
	D3D12_BLEND_DESC blendDesc[6]{};
	//ブレンドなし
	blendDesc[0].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//半透明合成
	blendDesc[1].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[1].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[1].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[1].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[1].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[1].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	blendDesc[1].RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値

	//加算合成
	blendDesc[2].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[2].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[2].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[2].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[2].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[2].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[2].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc[2].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//減算合成
	blendDesc[3].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[3].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[3].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[3].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[3].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[3].RenderTarget[0].BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//減算
	blendDesc[3].RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc[3].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う

	//乗算合成
	blendDesc[4].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[4].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[4].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[4].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[4].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[4].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[4].RenderTarget[0].SrcBlend = D3D12_BLEND_ZERO;//ソースの値を0%使う
	blendDesc[4].RenderTarget[0].DestBlend = D3D12_BLEND_SRC_COLOR;//デストの色を使う

	//スクリーン合成
	blendDesc[5].RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc[5].RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc[5].RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc[5].RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc[5].RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	blendDesc[5].RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc[5].RenderTarget[0].SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1-ソースの色を使う
	blendDesc[5].RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの色を100%使う


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = shaderCompiler_->CompileShader(L"SpriteVS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = shaderCompiler_->CompileShader(L"SpritePS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = true;
	//書き込みします
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//書き込むRTVの情報
	DXGI_FORMAT rtvFormats[1]{};
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;


	//PSOを作成する
	for (uint32_t i = 0; i < kCountOfBlendMode; i++) {
		spritePipelineStates_.push_back(std::make_unique<PipelineState>());
		spritePipelineStates_[i]->SetRootSignature(spriteRootSignature_.get());
		spritePipelineStates_[i]->SetInputLayout(2, inputElementDescs);
		spritePipelineStates_[i]->SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
		spritePipelineStates_[i]->SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
		spritePipelineStates_[i]->SetBlendState(blendDesc[i]);
		spritePipelineStates_[i]->SetRasterizerState(rasterizerDesc);
		spritePipelineStates_[i]->SetRenderTargetFormat(1, rtvFormats);
		spritePipelineStates_[i]->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		spritePipelineStates_[i]->SetSampleDesc(1);
		spritePipelineStates_[i]->SetDepthStencilState(depthStencilDesc);
		spritePipelineStates_[i]->SetDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);
		spritePipelineStates_[i]->Finalize();
	}
}

void Renderer::CreateParticlePipelineState() {
	//生成
	particleRootSignature_ = std::make_unique<RootSignature>();

	//DescriptorRange作成
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//Offsetを自動計算

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;//数は一つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootParameter作成。複数設定できるので配列。今回は結果一つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVで使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド
	particleRootSignature_->AddRootParameter(rootParameters[0]);
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;//Tableの中身の配列を指定
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);//Tableで利用する数
	particleRootSignature_->AddRootParameter(rootParameters[1]);
	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[2].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う
	particleRootSignature_->AddRootParameter(rootParameters[2]);
	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderで使う
	rootParameters[3].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数
	particleRootSignature_->AddRootParameter(rootParameters[3]);

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
	particleRootSignature_->AddStaticSampler(staticSamplers[0]);
	//バイナリを元に生成
	particleRootSignature_->Finalize();


	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blendDesc.RenderTarget[0].BlendEnable = true;//ブレンドを有効にする
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;//デストの値を0%使う
	//加算合成
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースの値を100%使う
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;//デストの値を100%使う
	////半透明合成
	//blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;//加算
	//blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;//ソースのアルファ値
	//blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-ソースのアルファ値


	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;


	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = shaderCompiler_->CompileShader(L"Particle.VS.hlsl", L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = shaderCompiler_->CompileShader(L"Particle.PS.hlsl", L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を無効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みしない
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//書き込むRTVの情報
	DXGI_FORMAT rtvFormats[2]{};
	rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvFormats[1] = DXGI_FORMAT_R32_FLOAT;


	//PSOを作成する
	particlePipelineStates_.push_back(std::make_unique<PipelineState>());
	particlePipelineStates_[0]->SetRootSignature(particleRootSignature_.get());
	particlePipelineStates_[0]->SetInputLayout(3, inputElementDescs);
	particlePipelineStates_[0]->SetVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize());
	particlePipelineStates_[0]->SetPixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize());
	particlePipelineStates_[0]->SetBlendState(blendDesc);
	particlePipelineStates_[0]->SetRasterizerState(rasterizerDesc);
	particlePipelineStates_[0]->SetRenderTargetFormat(2, rtvFormats);
	particlePipelineStates_[0]->SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
	particlePipelineStates_[0]->SetSampleDesc(1);
	particlePipelineStates_[0]->SetDepthStencilState(depthStencilDesc);
	particlePipelineStates_[0]->SetDepthStencilFormat(DXGI_FORMAT_D24_UNORM_S8_UINT);
	particlePipelineStates_[0]->Finalize();
}