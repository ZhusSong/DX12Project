#include "GameApp.h"



GameApp::GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
    : DX12App(hInstance, windowName, initWidth, initHeight)
{
}



GameApp::~GameApp()
{
}

bool GameApp::Init()
{
    if (!DX12App::Init())
        return false;


    return true;
}

void GameApp::OnResize()
{
    DX12App::OnResize();
}

void GameApp::Update(const DXGameTimer& gt)
{
}

void GameApp::Draw(const DXGameTimer& gt)
{
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    // 重复使用记录命令的相关内存
    // 只有当与GPU关联的命令列表执行完成时才将其重置
    ThrowIfFailed(mDirectCmdListAlloc->Reset());

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    // 将某个命令列表加入命令队列后，便重置该命令列表以此来复用命令列表及其内存
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

    // Indicate a state transition on the resource usage.
    // 对资源状态进行转换，将资源从呈现状态转换为渲染目标状态
    mCommandList->ResourceBarrier(1,&CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,D3D12_RESOURCE_STATE_RENDER_TARGET));

    // Set the viewport and scissor rect.  This needs to be reset whenever the command list is reset.
    // 设置视口与裁剪矩阵，它们需要随着命令列表的重置而重置
    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Clear the back buffer and depth buffer.
    // 清空后台缓冲区与深度缓冲区
    mCommandList->ClearRenderTargetView(CurrentBackBufferView(),Colors::LightSteelBlue,0,nullptr);
    mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    // 指定将要渲染的缓冲区
    mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

    // Indicate a state transition on the resource usage.
    // 再次对资源文件状态进行转换，将资源从渲染目标状态转换为呈现状态
    mCommandList->ResourceBarrier(
        1, &CD3DX12_RESOURCE_BARRIER::Transition(
            CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

    // Done recording commands.
    // 完成对命令的记录
    ThrowIfFailed(mCommandList->Close());

    // Add the command list to the queue for execution.
    // 将待执行的命令列表加入命令队列
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // swap the back and front buffers
    // 交换后台缓冲区与前台缓冲区
    ThrowIfFailed(mSwapChain->Present(0, 0));
    mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

    // Wait until frame commands are complete.  
    // 等待此帧的命令执行完毕。
    FlushCommandQueue();
}