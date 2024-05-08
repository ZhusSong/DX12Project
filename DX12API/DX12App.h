#pragma once
#ifndef DX12App_H
#define DX12App_H

class DX11App
{
public:
    //初始化
     //Initialisation
     //初期化
    DX11App(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);

    virtual ~DX11App();

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
}
