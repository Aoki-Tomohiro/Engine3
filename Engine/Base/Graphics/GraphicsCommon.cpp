#include "GraphicsCommon.h"
#include <cassert>

//実体定義
GraphicsCommon* GraphicsCommon::instance = nullptr;

GraphicsCommon* GraphicsCommon::GetInstance() {
	if (instance == nullptr) {
		instance = new GraphicsCommon();
	}
	return instance;
}

void GraphicsCommon::DeleteInstance() {
	delete instance;
	instance = nullptr;
}

void GraphicsCommon::Initialize() {
	//WindowsApplicationのインスタンスを取得
	app_ = Application::GetInstance();

	//FPS固定初期化
	InitializeFixFPS();

	//デバイスの作成
	CreateDevice();

	//コマンドの作成
	CreateCommand();

	//スワップチェーンの作成
	CreateSwapChain();

	//フェンスの作成
	CreateFence();

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
	TransitionResource(*swapChainResource_[backBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET);

	//ディスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	rtvHandles[0] = swapChainResource_[0]->GetRTVHandle();
	rtvHandles[1] = swapChainResource_[1]->GetRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilResource_->GetDSVHandle();
	//描画先のRTVとDSVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);

	//指定した色で画面全体をクリアする
	ClearRenderTarget();

	//指定した深度で画面全体をクリアする
	ClearDepthBuffer();

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = app_->kClientWidth;
	viewport.Height = app_->kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	//ビューポートを設定
	commandList_->RSSetViewports(1, &viewport);

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = app_->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = app_->kClientHeight;
	//シザーを設定
	commandList_->RSSetScissorRects(1, &scissorRect);
}

void GraphicsCommon::PostDraw() {
	//バックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	//リソースの状態遷移
	TransitionResource(*swapChainResource_[backBufferIndex], D3D12_RESOURCE_STATE_PRESENT);

	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));
	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	//GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);

	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
	//Fenceの値が指定したSignal値にたどり着いているか確認する
	//GetCompletedValueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceValue_) {
		//FenceのSignalを待つためのイベントを作成する
		HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		assert(fenceEvent != nullptr);
		//指定したSignalにたどり着いていないので、たどり着くまで待つようにイベントを設定する
		fence_->SetEventOnCompletion(fenceValue_, fenceEvent);
		//イベントを待つ
		WaitForSingleObject(fenceEvent, INFINITE);
		//Fenceのイベントを閉じる
		CloseHandle(fenceEvent);
	}

	//FPS固定
	UpdateFixFPS();

	//次のフレーム用のコマンドリストを準備
	hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}

void GraphicsCommon::ClearRenderTarget() {
	//バックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
	//ディスクリプタハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2]{};
	rtvHandles[0] = swapChainResource_[0]->GetRTVHandle();
	rtvHandles[1] = swapChainResource_[1]->GetRTVHandle();
	//指定した色で画面全体をクリアする
	const float* clearColor = swapChainResource_[backBufferIndex]->GetClearColor();
	commandList_->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
}

void GraphicsCommon::ClearDepthBuffer() {
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = depthStencilResource_->GetDSVHandle();
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void GraphicsCommon::TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState) {
	//現在のリソースの状態を取得
	D3D12_RESOURCE_STATES currentState = resource.GetResourceState();

	//リソースの状態遷移
	if (currentState != newState) {
		//TransitionBarrierの設定
		D3D12_RESOURCE_BARRIER barrier{};
		//今回のバリアはTransition
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		//Noneにしておく
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		//バリアを張る対象のリソース。現在のバックバッファに行う
		barrier.Transition.pResource = resource.GetResource();
		//サブリソース
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		//遷移前(現在)のResourceState
		barrier.Transition.StateBefore = currentState;
		//遷移後のResourceState
		barrier.Transition.StateAfter = newState;
		//TransitionBarrierを張る
		commandList_->ResourceBarrier(1, &barrier);
		//リソースの状態を更新する
		resource.SetResourceState(newState);
	}
}

void GraphicsCommon::CreateDevice() {
#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//デバッグレイヤーを有効にする
		debugController->EnableDebugLayer();
		//さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	//DXGIファクトリーの作成
	//HRESULTはWindows系のエラーコードであり、//関数が成功したかどうかをSUCCEEDEDマクロで判定できる
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	//初期化の根本的な部分でエラーが出た場合はプログラムが間違っているか、//どうにもできない場合が多いのでassertにしておく
	assert(SUCCEEDED(hr));


	//良い順にアダプタを読む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		//アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr = useAdapter_->GetDesc3(&adapterDesc);
		//取得できなかったら止める
		assert(SUCCEEDED(hr));
		//ソフトウェアアダプタでなければ採用!
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//採用したアダプタの情報をログに出力
			Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
			break;
		}
		//ソフトウェアアダプタの場合は見なかったことにする
		useAdapter_ = nullptr;
	}
	//適切なアダプタが見つからなかったので起動できない
	assert(useAdapter_ != nullptr);


	//機能レベルとログ出力用の文字列
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,D3D_FEATURE_LEVEL_12_1,D3D_FEATURE_LEVEL_12_0
	};
	const char* featureLevelStrings[] = { "12.2","12.1","12.0" };
	//高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		//採用したアダプターでデバイス作成
		hr = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		//指定した機能レベルでデバイスが生成できたかを確認
		if (SUCCEEDED(hr)) {
			//生成できたのでログ出力を行ってループを抜ける
			Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	//デバイスの生成がうまくできなかったので起動できない
	assert(device_ != nullptr);
	//初期化完了のログを出す
	Log("Complete Create D3D12Device!!!\n");


#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//エラー時に泊まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//警告時に泊まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			//https://stackoverflow.com/questions/69805245/directx-12-application-is-craching-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);
	}
#endif
}

void GraphicsCommon::CreateCommand() {
	//コマンドキューを作成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの作成がうまくできなかったので起動できない
	assert(SUCCEEDED(hr));


	//コマンドアロケータを作成する
	hr = device_->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケーターの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));


	//コマンドリストを作成する
	hr = device_->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリストの作成がうまくできなかったので起動できない
	assert(SUCCEEDED(hr));
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
	HRESULT hr = dxgiFactory_->CreateSwapChainForHwnd(commandQueue_.Get(), app_->GetHwnd(), &swapChainDesc, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr));
}

void GraphicsCommon::CreateFence() {
	//初期値0でFenceを作る
	HRESULT hr = device_->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
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