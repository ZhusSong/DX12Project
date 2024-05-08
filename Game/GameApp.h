//游戏程序入口
#pragma once
#ifndef GAMEAPP_H
#define GAMEPAPP_H

#include "DX12App.h"
class GameApp : public DX12App
{
public:
    GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    ~GameApp();

    bool Init();
    void OnResize();
    void UpdateScene(float dt);
    void DrawScene();
};

#endif