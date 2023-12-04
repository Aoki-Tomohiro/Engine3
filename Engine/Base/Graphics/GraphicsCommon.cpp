#include "GraphicsCommon.h"

GraphicsCommon* GraphicsCommon::instance_ = nullptr;

GraphicsCommon* GraphicsCommon::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new GraphicsCommon();
	}
	return instance_;
}

void GraphicsCommon::DeleteInstance() {
	delete instance_;
	instance_ = nullptr;
	//GraphicsDeviceの開放
	GraphicsDevice::DeleteInstance();
	//GraphicsContextの開放
	GraphicsContext::DeleteInstance();
}

void GraphicsCommon::Initialize() {
	//Applicationのインスタンスを取得
	app_ = Application::GetInstance();

	//GraphicsDeviceの初期化
	graphicsDevice_ = GraphicsDevice::GetInstance();
	graphicsDevice_->Initialize();

	//GraphicsContextの初期化
	graphicsContext_ = GraphicsContext::GetInstance();
	graphicsContext_->Initialize(graphicsDevice_->GetDevice());

	//FPS固定初期化
	InitializeFixFPS();

	//スワップチェーンの生成
	CreateSwapChain();

	//ディスクリプタヒープの作成
	CreateDescriptorHeaps();

	//スワップチェーンのリソースを作成
	CreateSwapChainResources();

	//RTVを作成
	CreateRenderTargetViews();

	//DepthBufferとDSVの作成
	CreateDepthBufferAndView();
}

void GraphicsCommon::PreDraw() {
	//これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//リソースの状態遷移
	graphicsContext_->TransitionResource(*swapChainResource_[backBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET);

	//ディスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	rtvHandles[0] = swapChainResource_[0]->GetRTVHandle();
	rtvHandles[1] = swapChainResource_[1]->GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilResource_->GetDSVHandle();

	//描画先のRTVとDSVを設定する
	graphicsContext_->SetRenderTargets(1, &rtvHandles[backBufferIndex], dsvHandle);

	//指定した色で画面全体をクリアする
	graphicsContext_->ClearRenderTarget(*swapChainResource_[backBufferIndex]);

	//指定した深度で画面全体をクリアする
	graphicsContext_->ClearDepthBuffer(*depthStencilResource_);

	//ビューポートを設定
	graphicsContext_->SetViewport(app_->kClientWidth, app_->kClientHeight, 0, 0, 0.0f, 1.0f);

	//シザーを設定
	graphicsContext_->SetScissorRect(0, app_->kClientWidth, 0, app_->kClientHeight);
}

void GraphicsCommon::PostDraw() {
	//バックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//リソースの状態遷移
	graphicsContext_->TransitionResource(*swapChainResource_[backBufferIndex], D3D12_RESOURCE_STATE_PRESENT);

	//GPUにコマンドリストの実行を行わせる
	graphicsContext_->Execute();

	//GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	//GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	graphicsContext_->Signal();

	//Fenceが指定した値に到達するまで待機
	graphicsContext_->WaitForFence();

	//FPS固定
	UpdateFixFPS();

	graphicsContext_->Reset();
}

void GraphicsCommon::CreateSwapChain() {
	//スワップチェーンを作成する
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = app_->kClientWidth;//画面の幅。ウィンドウのクライアント領域と同じものにしておく
	swapChainDesc.Height = app_->kClientHeight;//画面の高さ。ウィンドウのクライアント領域と同じものにしておく
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//色の形式
	swapChainDesc.SampleDesc.Count = 1;//マルチサンプルしない
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//描画のターゲットとして利用する
	swapChainDesc.BufferCount = 2;//ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//モニタに映したら、中身を破棄
	//コマンドキュー、ウィンドウハンドル、設定を渡して生成する
	IDXGIFactory7* dxgiFactory = graphicsDevice_->GetDXGIFactory();
	ID3D12CommandQueue* commandQueue = graphicsContext_->GetCommandQueue();
	HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue, app_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void GraphicsCommon::CreateDescriptorHeaps() {
	//RTVDescriptorHeapの作成
	rtvDescriptorHeap_ = std::make_unique<RTVHeap>();
	rtvDescriptorHeap_->Create(2);

	//DSVDescriptorHeapの作成
	dsvDescriptorHeap_ = std::make_unique<DSVHeap>();
	dsvDescriptorHeap_->Create(1);
}

void GraphicsCommon::CreateSwapChainResources() {
	// クリアカラー
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };  // 青っぽい色。RGBAの順

	// リソースの作成
	for (uint32_t i = 0; i < 2; i++) {
		swapChainResource_[i] = std::make_unique<ColorBuffer>();
		swapChainResource_[i]->CreateFromSwapChain(swapChain_.Get(), i, clearColor);
	}
}

void GraphicsCommon::CreateRenderTargetViews() {
	// レンダーターゲットビューの作成
	for (uint32_t i = 0; i < 2; i++) {
		rtvDescriptorHeap_->CreateRenderTargetView(*swapChainResource_[i], DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	}
}

void GraphicsCommon::CreateDepthBufferAndView() {
	// DepthBufferの作成
	depthStencilResource_ = std::make_unique<DepthBuffer>();
	depthStencilResource_->Create(app_->kClientWidth, app_->kClientHeight, DXGI_FORMAT_D24_UNORM_S8_UINT);

	// DepthStencilViewの作成
	dsvDescriptorHeap_->CreateDepthStencilView(*depthStencilResource_, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

void GraphicsCommon::InitializeFixFPS() {
	//現在時間を記録する
	reference_ = std::chrono::steady_clock::now();
}

void GraphicsCommon::UpdateFixFPS() {
	//1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));
	//1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	//現在時間を取得する
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	//前回記録からの経過時間を取得する
	std::chrono::microseconds elapsed = std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	//1/60(よりわずかに短い時間)経っていない場合
	if (elapsed < kMinCheckTime) {
		//1/60秒経過するまで微小なスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			//1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}
	//現在の時間を記録する
	reference_ = std::chrono::steady_clock::now();
}