//游戏程序入口
#pragma once
#ifndef GAMEAPP_H
#define GAMEPAPP_H
#include <DirectXColors.h>

#include "DX12App.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
using namespace DirectX;
using namespace DirectX::PackedVector;
class GameApp : public DX12App
{
public:

    struct Vertex
    {
        XMFLOAT3 Pos;
        XMFLOAT4 Color;
    };
    struct ObjectConstants
    {
        XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
    };
    GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    ~GameApp();

   virtual bool Init()override;
   
private:
    virtual void OnResize()override;
    virtual void Update(const DXGameTimer& gt)override;
    virtual void Draw(const DXGameTimer& gt)override;

    void DrawGame();

    // 创建常量缓冲区描述符堆
    void BuildDescriptorHeaps();
    // 创建常量缓冲区
    void BuildConstantBuffers();
    //创建根签名
    void BuildRootSignature();
    //创建Shader与输入布局
    void BuildShadersAndInputLayout();
    //创建盒子几何体
    void BuildBoxGeometry();
    //创建PSO(流水线状态对象)
    void BuildPSO();
private:
    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 5.0f;

    POINT mLastMousePos;
};

#endif