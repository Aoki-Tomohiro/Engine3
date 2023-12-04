#include "EngineCore.h"
#include "Engine/3D/Model/Model.h"
#include "Engine/2D/Sprite/Sprite.h"
#include "Engine/Components/Particle/ParticleSystem.h"
#include "Engine/Utilities/GlobalVariables.h"

EngineCore::~EngineCore() {
	//シーンマネージャの開放
	SceneManager::DeleteInstance();

	//Audioの開放
	audio_->Finalize();

	//Renderer開放
	Renderer::DeleteInstance();

	//ImGuiの開放
	imguiManager_->ShutDown();

	//テクスチャマネージャーの開放
	TextureManager::DeleteInstnace();

	//GraphicsCoreの解放
	GraphicsCommon::DeleteInstance();

	//ゲームウィンドウを閉じる
	app_->CloseGameWindow();
}

void EngineCore::Initialize() {
	//ゲームウィンドウの作成
	app_ = Application::GetInstance();
	app_->CreateGameWindow(L"DirectXGame", Application::kClientWidth, Application::kClientHeight);

	//GraphicsCoreの初期化
	graphicsCommon_ = GraphicsCommon::GetInstance();
	graphicsCommon_->Initialize();

	//テクスチャマネージャーの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	//ImGuiManagerの初期化
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize();

	//Rendererの初期化
	renderer_ = Renderer::GetInstance();
	renderer_->Initialize();

	//Inputの初期化
	input_ = Input::GetInstance();
	input_->Initialize();

	//Audioの初期化
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	//モデルの静的初期化
	Model::StaticInitialize();

	//スプライトの静的初期化
	Sprite::StaticInitialize();

	//パーティクルシステムの静的初期化
	ParticleSystem::StaticInitialize();

	//シーンマネージャーのインスタンスを取得
	sceneManager_ = SceneManager::GetInstance();
}

void EngineCore::Update() {
	//Inputの更新
	input_->Update();

	//ImGui受付開始
	imguiManager_->Begin();

	//グローバル変数の更新
	GlobalVariables::GetInstance()->Update();

	//シーンの更新
	sceneManager_->Update();

	//ImGui受付終了
	imguiManager_->End();
}

void EngineCore::Draw() {
	//描画前処理
	graphicsCommon_->PreDraw();

	//シーンの描画
	sceneManager_->Draw();

	//スプライトの描画
	sceneManager_->DrawUI();

	//ImGuiの描画
	imguiManager_->Draw();

	//描画後処理
	graphicsCommon_->PostDraw();
}

bool EngineCore::IsEndRequest() {
	//終了リクエストを取得
	bool endRequest = app_->ProcessMessage();
	return endRequest;
}

void EngineCore::Run() {
	//初期化
	Initialize();

	//ゲームループ
	while (true) {
		//ゲームループを抜ける
		if (IsEndRequest()) {
			break;
		}

		//更新
		Update();

		//描画
		Draw();
	}
}