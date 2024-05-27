#include "GameApp.h"
#include "MathHelper.h"


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

    // Reset the command list to prep for initialization commands.
    // 重置命令列表为执行初始化命令做好准备
    ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildBoxGeometry();
    BuildPSO();

    // Execute the initialization commands.
    // 执行初始化命令
    ThrowIfFailed(mCommandList->Close());
    ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    // 等待初始化完成
    FlushCommandQueue();

    return true;
}

void GameApp::OnResize()
{
    DX12App::OnResize();
    // The window resized, so update the aspect ratio and recompute the projection matrix.
    // 若窗口尺寸被调整，则更新纵横比并重新计算投影矩阵
    XMMATRIX P=XMMatrixPerspectiveFovLH(0.25f*MathHelper::Pi,AspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);

}

void GameApp::Update(const DXGameTimer& gt)
{
    // Convert Spherical to Cartesian coordinates.
    // 将球坐标转换为笛卡尔坐标
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    // Build the view matrix.
    // 构建观察矩阵
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, view);

    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProj = world * view * proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    // 使用最新的 worldViewProj 矩阵来更新常量缓冲区
    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    mObjectCB->CopyData(0, objConstants);
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

    //绘制游戏物体及画面
    DrawGame();


    // Done recording commands.
    // 完成对命令的记录,需在此之前进行游戏物体及逻辑的绘制
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

void GameApp::DrawGame()
{
    bool show_demo_window = true;
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);
    static int counter = 0;
    //test1    控制旋转
    ImGui::Begin("WangMeifo!Test");                          // Create a window called "Hello, world!" and append into it.

    ImGui::Text("Drag the slider to rotate the Angle of the Box.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state

    //ImGui::SliderFloat("float", &mPhi, 0.1f, 1.0f);  //mPhi立方体的旋转角度
  //  ImGui::SliderFloat("float", &mTheta, 0.1f, 1.0f);  //mTheta也是旋转角度
    // Edit 1 float using a slider from 0.0f to 1.0f

    if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
        counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Render();
    mCommandList->SetDescriptorHeaps(1, mSrvHeap.GetAddressOf());
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), mCommandList.Get());
}
