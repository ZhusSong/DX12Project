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
public:
    //初始化
     //Initialisation
     //初期化
    DX12App(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);

    virtual ~DX12App();

    //获取应用实例的句柄
    HINSTANCE AppInstance()const;
    //获取主窗口句柄
    HWND      MainWnd()const;
    //获取屏幕宽高比
    float     FormRatio()const;

    //运行
    int Run();

    //初始化窗口及Direct3D部分
    virtual bool Init();
    //在窗口大小变动时调用
    virtual void OnResize();
    //实现每一帧更新
    virtual void UpdateScene(float dt) = 0;
    //实现每一帧绘制
    virtual void DrawScene() = 0;
    //窗口消息回调
    virtual LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //鼠标位置测试
    float m_MousePosX;
    float m_MousePosY;


protected:
    //窗口初始化
    bool InitMainWindow();
    // Direct2D初始化
    bool InitDirect2D();
    //Direct3D初始化
    bool InitDirect3D();

    bool InitImGui();
    //显示帧数
    void ShowFrameCount();

protected:
    //窗口初始化
    bool InitMainWindow();
    // Direct2D初始化
    bool InitDirect2D();
    //Direct3D初始化
    bool InitDirect3D();

    bool InitImGui();
    //显示帧数
    void ShowFrameCount();

    //应用实例句柄
    HINSTANCE m_AppInstance;
    //主窗口
    HWND      m_MainWnd;
    //是否暂停
    bool      m_AppPaused;
    //是否最小化
    bool      m_Minimized;
    //是否最大化
    bool      m_Maximized;
    //窗口大小是否变化
    bool      m_Resizing;
    //是否开启4倍多重采样
    bool      m_Enable4xMsaa;
    //MSAA质量等级
    UINT      m_4xMsaaQuality;

    //计时器
    DXGameTimer m_Timer;

    //模板
    template <class T>
    using ComPtr = Microsoft::WRL::ComPtr<T>;
};
#endif