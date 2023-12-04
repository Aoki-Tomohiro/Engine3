#include "GraphicsContext.h"
#include <cassert>

GraphicsContext* GraphicsContext::instance_ = nullptr;

GraphicsContext* GraphicsContext::GetInstance() {
	if (instance_ == nullptr) {
		instance_ = new GraphicsContext();
	}
	return instance_;
}

void GraphicsContext::DeleteInstance() {
	delete instance_;
	instance_ = nullptr;
}

void GraphicsContext::Initialize(ID3D12Device* device) {
	//コマンドキューを作成する
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	HRESULT hr = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));
	//コマンドキューの作成がうまくできなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドアロケータを作成する
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	//コマンドアロケーターの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr));

	//コマンドリストを作成する
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr, IID_PPV_ARGS(&commandList_));
	//コマンドリストの作成がうまくできなかったので起動できない
	assert(SUCCEEDED(hr));

	//フェンスの作成
	hr = device->CreateFence(fenceValue_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr));
}

void GraphicsContext::TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState) {
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

void GraphicsContext::SetRenderTargets(UINT count, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[]) {
	commandList_->OMSetRenderTargets(count, rtvHandles, false, nullptr);
}

void GraphicsContext::SetRenderTargets(UINT count, const D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[], D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) {
	commandList_->OMSetRenderTargets(count, rtvHandles, false, &dsvHandle);
}

void GraphicsContext::ClearRenderTarget(ColorBuffer& resource) {
	commandList_->ClearRenderTargetView(resource.GetRTVHandle(), resource.GetClearColor(), 0, nullptr);
}

void GraphicsContext::ClearDepthBuffer(DepthBuffer& resource) {
	commandList_->ClearDepthStencilView(resource.GetDSVHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void GraphicsContext::SetViewport(FLOAT width, FLOAT height, FLOAT topLeftX, FLOAT topLeftY, FLOAT minDepth, FLOAT maxDepth) {
	D3D12_VIEWPORT viewport{};
	viewport.Width = width;
	viewport.Height = height;
	viewport.TopLeftX = topLeftX;
	viewport.TopLeftY = topLeftY;
	viewport.MinDepth = minDepth;
	viewport.MaxDepth = maxDepth;
	commandList_->RSSetViewports(1, &viewport);
}

void GraphicsContext::SetScissorRect(UINT left, UINT right, UINT top, UINT bottom) {
	D3D12_RECT scissorRect{};
	scissorRect.left = left;
	scissorRect.right = right;
	scissorRect.top = top;
	scissorRect.bottom = bottom;
	commandList_->RSSetScissorRects(1, &scissorRect);
}

void GraphicsContext::SetVertexBuffer(const D3D12_VERTEX_BUFFER_VIEW& vertexBufferView) {
	commandList_->IASetVertexBuffers(0, 1, &vertexBufferView);
}

void GraphicsContext::SetPrimitiveTopology(const D3D_PRIMITIVE_TOPOLOGY& primitiveTopology) {
	commandList_->IASetPrimitiveTopology(primitiveTopology);
}

void GraphicsContext::SetConstantBuffer(UINT rootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS gpuAddress) {
	commandList_->SetGraphicsRootConstantBufferView(rootParameterIndex, gpuAddress);
}

void GraphicsContext::SetDescriptorTable(UINT rootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle) {
	commandList_->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuHandle);
}

void GraphicsContext::SetDescriptorHeaps(UINT count, ID3D12DescriptorHeap* descriptorHeap[]) {
	commandList_->SetDescriptorHeaps(count, descriptorHeap);
}

void GraphicsContext::SetRootSignature(const Microsoft::WRL::ComPtr<ID3D12RootSignature>& rootSignature) {
	commandList_->SetGraphicsRootSignature(rootSignature.Get());
}

void GraphicsContext::SetPipelineState(const Microsoft::WRL::ComPtr<ID3D12PipelineState>& pipelineState) {
	commandList_->SetPipelineState(pipelineState.Get());
}

void GraphicsContext::DrawInstanced(UINT vertexCount, UINT instanceCount) {
	commandList_->DrawInstanced(vertexCount, instanceCount, 0, 0);
}

void GraphicsContext::Execute() {
	//コマンドリストの内容を確定させる。すべてのコマンドを積んでからCloseすること
	HRESULT hr = commandList_->Close();
	assert(SUCCEEDED(hr));
	//GPUにコマンドリストの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
}

void GraphicsContext::Signal() {
	//Fenceの値を更新
	fenceValue_++;
	//GPUがここまでたどり着いた時に、Fenceの値を指定した値に代入するようにSignalを送る
	commandQueue_->Signal(fence_.Get(), fenceValue_);
}

void GraphicsContext::WaitForFence() {
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
}

void GraphicsContext::Reset() {
	//次のフレーム用のコマンドリストを準備
	HRESULT hr = commandAllocator_->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr));
}