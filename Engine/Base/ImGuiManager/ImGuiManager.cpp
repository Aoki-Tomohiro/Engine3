#include "ImGuiManager.h"
#include "Engine/Base/Graphics/GraphicsContext.h"

ImGuiManager* ImGuiManager::GetInstance() {
	static ImGuiManager instance;
	return &instance;
}

void ImGuiManager::Initialize() {
	//ウィンドウズアプリケーションのインスタンスを取得
	app_ = Application::GetInstance();
	//DirectXCommonのインスタンスを取得
	device_ = GraphicsDevice::GetInstance();

	//SRVDescriptorHeapの作成
	srvDescriptorHeap_ = std::make_unique<SRVHeap>();
	srvDescriptorHeap_->Create(1);

	//ImGuiの初期化
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(app_->GetHwnd());
	ImGui_ImplDX12_Init(device_->GetDevice(),
		2,
		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		srvDescriptorHeap_->GetDescriptorHeap(),
		srvDescriptorHeap_->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap_->GetDescriptorHeap()->GetGPUDescriptorHandleForHeapStart());
}

void ImGuiManager::Begin() {
	//ImGuiにフレームの開始を伝える
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void ImGuiManager::End() {
	//ImGuiの内部コマンドを生成する
	ImGui::Render();
}

void ImGuiManager::Draw() {
	//コマンドリストを取得
	GraphicsContext* graphicsContext = GraphicsContext::GetInstance();

	//描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap_->GetDescriptorHeap() };
	graphicsContext->SetDescriptorHeaps(1, descriptorHeaps);

	//実際にcommandListのImGuiの描画コマンドを積む
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), graphicsContext->GetCommandList());
}

void ImGuiManager::ShutDown() {
	//解放処理
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	srvDescriptorHeap_.reset();
}