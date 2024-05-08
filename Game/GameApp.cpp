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

    return true;
}

void GameApp::OnResize()
{
}

void GameApp::UpdateScene(float dt)
{
}

void GameApp::DrawScene()
{
}