#include "PostEffects.h"

void PostEffects::Initialize() {
	//レンダラーのインスタンスを取得
	renderer_ = Renderer::GetInstance();
}