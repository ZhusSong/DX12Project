#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#ifndef DX12App_H
#define DX12App_H
#include <wrl/client.h>
#include <string>
#include <d2d1.h>
#include <d3d12.h>
#include <dwrite.h>
#include <DirectXMath.h>
#include "WinAPISetting.h"
#include "d3dUtil.h"
#include "DXGameTimer.h"
//添加ImGui
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"




class DX12App
{
    protected:
     //初始化,并设置单例
     //Initialisation
     //初期化
    DX12App(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    //禁止编译器使用拷贝构造函数创建DX12App的拷贝
    DX12App(const DX12App& rhs) = delete;
    //禁止编译器通过使用=来创建DX12App的拷贝
    DX12App& operator=(const DX12App& rhs) = delete;

    //释放所有ComPtr对象，使用构造函数进行销毁是为了保证在销毁之前，GPU能处理完所有指令
    virtual ~DX12App();
public:
    //单例 返回DX12App实例
    static DX12App* GetApp();

    //获取应用实例的句柄
    HINSTANCE AppInstance()const;
    //获取主窗口句柄
    HWND      MainWnd()const;
    //获取屏幕宽高比
    float    AspectRatio()const;

    //是否使用4X MSAA功能
    bool Get4xMsaaState()const;
    //开启或禁用4X MSAA功能
    void Set4xMsaaState(bool value);

    //运行
    int Run();

    //初始化窗口及Direct3D部分,用于在派生类中进行初始化
    virtual bool Init();
    //窗口消息回调
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
protected:
    //创建RTV与DSV描述符
    virtual void CreateRtvAndDsvDescriptorHeaps();

    //在窗口大小变动时调用，获取变化后的屏幕宽高比并进行后台缓冲区的重新创建
    virtual void OnResize();

    //实现每一帧更新
    virtual void Update(const DXGameTimer& gt) = 0;

    //实现每一帧绘制
    virtual void Draw(const DXGameTimer& gt) = 0;


    //使用ImGui进行输入检测，此处不需要
    // Convenience overrides for handling mouse input.
    //virtual void OnMouseDown(WPARAM btnState, int x, int y) { }
    //virtual void OnMouseUp(WPARAM btnState, int x, int y) { }
    //virtual void OnMouseMove(WPARAM btnState, int x, int y) { }

    //鼠标位置测试
    float m_MousePosX;
    float m_MousePosY;

    //初始化主窗口
    bool InitMainWindow();
    // Direct2D初始化
    bool InitDirect2D();
    //Direct3D初始化
    bool InitDirect3D();
    //创建命令队列，命令列表分配器与命令列表
    void CreateCommandObjects();

    //创建交换链
    void CreateSwapChain();

    //强制CPU等待GPU，直到GPU处理完队列中所有命令
    void FlushCommandQueue();
    //返回交换链中当前后台缓冲区的ID3D12Resource
    ID3D12Resource* CurrentBackBuffer()const;
    //返回当前后台缓冲区的RTV(渲染目标视图，render target view)
    D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
    //返回主深度/模板缓冲区的DSV(深度/模板视图，depth/stencil view)
    D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;


    //显示帧数
    void ShowFrameCount();
    //枚举系统中所有适配器
    void LogAdapters();
    //枚举指定适配器的全部显示输出
    void LogAdapterOutputs(IDXGIAdapter* adapter);
    //枚举某个显示输出对特定格式支持的所有显示模式
    void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
    //初始化ImGui
    bool InitImGui();
protected:

    static DX12App* mApp;

    //应用实例句柄
    HINSTANCE mhAppInstance = nullptr;
    //主窗口
    HWND      mhMainWnd = nullptr;
    //是否暂停
    bool      mAppPaused = false;
    //是否最小化
    bool      mMinimized = false;
    //是否最大化
    bool      mMaximized = false;
    //窗口大小是否变化
    bool      mResizing = false;
    // fullscreen enabled
    //是否全屏
    bool      mFullscreenState = false;
    //是否开启4倍多重采样
    bool      m4xMsaaState=false;
    //MSAA质量等级
    UINT      m4xMsaaQuality=0;

    //计时器
    DXGameTimer mTimer;

    //模板
    template <class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
    //工厂
    ComPtr<IDXGIFactory4> mdxgiFactory;
    //交换链
    ComPtr<IDXGISwapChain> mSwapChain;
    //d3d设备
    ComPtr<ID3D12Device> md3dDevice;

    ComPtr<ID3D12Fence> mFence;
    UINT64 mCurrentFence = 0;

    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
    ComPtr<ID3D12GraphicsCommandList> mCommandList;

    static const int SwapChainBufferCount = 2;
    int mCurrBackBuffer = 0;
    ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
    ComPtr<ID3D12Resource> mDepthStencilBuffer;

    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;

    D3D12_VIEWPORT mScreenViewport;
    D3D12_RECT mScissorRect;

    UINT mRtvDescriptorSize = 0;
    UINT mDsvDescriptorSize = 0;
    UINT mCbvSrvUavDescriptorSize = 0;

    // Derived class should set these in derived constructor to customize starting values.
    // 用户应在派生类的派生构造函数中定义这些初始值
    //窗口名
    std::wstring mMainWndCaption = L"d3d App";
    //窗口宽度
    int mClientWidth;
    //窗口高度
    int mClientHeight;
    D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

};
#endif