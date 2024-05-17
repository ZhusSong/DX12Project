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

}