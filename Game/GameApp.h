//游戏程序入口
#pragma once
#ifndef GAMEAPP_H
#define GAMEPAPP_H
#include <DirectXColors.h>

#include "DX12App.h"
using namespace DirectX;
class GameApp : public DX12App
{
public:

    //摄像机模式
    enum class CameraMode {
        FirstPerson,
        ThirdPerson,
        Free,
    };

    GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    ~GameApp();

   virtual bool Init()override;
   
private:
    virtual void OnResize()override;
    virtual void Update(const DXGameTimer& gt)override;
    virtual void Draw(const DXGameTimer& gt)override;
};

#endif