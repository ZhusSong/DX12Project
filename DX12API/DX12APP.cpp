#include "DX12App.h"
#include <sstream>
#pragma warning(disable: 6031)
extern "C"
{
    // 在具有多显卡的硬件设备中，优先使用NVIDIA或AMD的显卡运行
    // 需要在.exe中使用
    __declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 0x00000001;
}
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
    return g_d3dApp->MsgProc(hwnd, msg, wParam, lParam);
}

DX12App::DX12App(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight)
    : m_AppInstance(hInstance),
    m_MainWndName(windowName),
    m_ViewWidth(initWidth),
    m_ViewHeight(initHeight),
    m_MainWnd(nullptr),
{

    // 让一个全局指针获取这个类，这样我们就可以在Windows消息处理的回调函数
    // 让这个类调用内部的回调函数了
    g_d3dApp = this;
}
