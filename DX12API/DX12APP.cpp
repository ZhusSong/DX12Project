#include "DX12App.h"
#include <WindowsX.h>
#include <sstream>

#pragma warning(disable: 6031)
using namespace std;
using namespace DirectX;
extern "C"
{
    // 在具有多显卡的硬件设备中，优先使用NVIDIA或AMD的显卡运行
    // 需要在.exe中使用
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}
//Im
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace
{
    // This is just used to forward Windows messages from a global window
    // procedure to our member function window procedure because we cannot
    // assign a member function to WNDCLASS::lpfnWndProc.
    DX12App* g_d3dApp = nullptr;
}

LRESULT CALLBACK
MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Forward hwnd on because we can get messages (e.g., WM_CREATE)
    // before CreateWindow returns, and thus before m_hMainWnd is valid.
    return DX12App::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

DX12App* DX12App::mApp = nullptr;
DX12App*DX12App::GetApp()
{
    return mApp;
}

DX12App::DX12App(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
{
    //单例，全局只能存在一个
    // Only one D3DApp can be constructed.
    assert(mApp == nullptr);
    mApp = this;
}
DX12App::~DX12App()
{
    if (md3dDevice != nullptr)
        FlushCommandQueue();
}
HINSTANCE DX12App::AppInstance()const
{
    return mhAppInstance;
}
HWND DX12App::MainWnd()const
{
    return mhMainWnd;
}
float DX12App::AspectRatio()const
{
    return static_cast<float>(mClientWidth) / mClientHeight;
}
bool  DX12App::Get4xMsaaState()const
{
    return m4xMsaaState;
}
void  DX12App::Set4xMsaaState(bool value)
{
    if (m4xMsaaState != value)
    {
        m4xMsaaState = value;
        // Recreate the swapchain and buffers with new multisample settings.
        //重设交换链与缓冲区
        CreateSwapChain();
        OnResize();
    }
}
int DX12App::Run()
{
    MSG msg = { 0 };
    mTimer.Reset();
    while (msg.message != WM_QUIT)
    {
        // If there are Window messages then process them.
        // 若有窗口信息则先处理窗口信息
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Otherwise, do animation/game stuff.
        // 若无窗口信息则处理游戏进程
        else
        {
            mTimer.GetTick();
            //若当前程序未暂停则进行更新与绘制
            if (!mAppPaused)
            {
                ShowFrameCount();
                Update(mTimer);
                Draw(mTimer);
            }
            else
            {
                Sleep(100);
            }
        }
    }
    return (int)msg.wParam;
}
bool DX12App::Init()
{
    if (!InitMainWindow())
        return false;

    if (!InitDirect3D())
        return false;

    // Do the initial resize code.
    // 按照初始宽高比进行创建
    OnResize();

    return true;
}
void DX12App::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
        &rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
        &dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}