//游戏程序入口
#pragma once
#ifndef GAMEAPP_H
#define GAMEPAPP_H
#include <cassert>
#include <DirectXColors.h>
#include <ppl.h>
#include <vector>
#include <algorithm>

#include "DX12App.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
#include "FrameResource.h"
#include "GeometryGenerator.h"

#include"Wave.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;


// Lightweight structure stores parameters to draw a shape.This will vary from app-to-app.
// 存储绘制图形所需的轻量结构体，随着应用程序的不同有所差别
struct RenderItem
{
    RenderItem() = default;

    // World matrix of the shape that describes the object's local space
    // relative to the world space, which defines the position, orientation,
    // and scale of the object in the world.
    // 描述物体局部空间相对于世界空间的世界矩阵
    // 定义了物体位于世界空间中的位置、朝向与大小
    XMFLOAT4X4 World = MathHelper::Identity4x4();
    XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

    // Dirty flag indicating the object data has changed and we need to update the constant buffer.
    // Because we have an object cbuffer for each FrameResource, we have to apply the
    // update to each FrameResource.  Thus, when we modify obect data we should set 
    // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
    // 用已更新标志(Dirty flag)来表示物体的相关数据已发生改变，这意味着我们此时需要更新
    // 常量缓冲区。由于每个FrameResource中都有一个物体常量缓冲区，所以我们必须对每个FR都进行更新
    // 即当我们修改物体数据的时候，应该按NumFrameDirty=gNumFrameResources进行设置
    int NumFramesDirty = gNumFrameResources;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    // 该索引指向的GPU常量缓冲区对应于当前渲染项中的物体常量缓冲区
    UINT ObjCBIndex = -1;

    // 此轩然项参与绘制的几何体，绘制一个几何体可能会用到多个渲染项
    Material* Mat = nullptr;
    MeshGeometry* Geo = nullptr;

    // Primitive topology.
    // 图元拓扑
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    // 所需参数
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};

enum class RenderLayer : int
{
    Opaque = 0,
    Count
};

class GameApp : public DX12App
{
public:

    GameApp(HINSTANCE hInstance, const std::wstring& windowName, int initWidth, int initHeight);
    GameApp(const  GameApp& rhs) = delete;
    GameApp& operator=(const  GameApp& rhs) = delete;
    ~GameApp();

   virtual bool Init()override;
   
private:
    virtual void OnResize()override;
    virtual void Update(const DXGameTimer& gt)override;
    virtual void Draw(const DXGameTimer& gt)override;

    void OnMouseDown();
    void OnMouseUp();
    void OnMouseMove();

    void OnKeyBoardInput(const DXGameTimer& gt);

    void UpdateCamera(const DXGameTimer& gt);
    void UpdateObjectCBs(const DXGameTimer& gt);
    void UpdateMaterialCBs(const DXGameTimer& gt);
    void UpdateMainPassCB(const DXGameTimer& gt);
    void UpdateWaves(const DXGameTimer& gt);

    void DrawGame();

    //创建根签名
    void BuildRootSignature();
    //创建Shader与输入布局
    void BuildShadersAndInputLayout();

    //创建陆地几何体
    void BuildLandGeometry();
    void BuildWavesGeometryBuffers();

    //创建PSO(流水线状态对象)
    void BuildPSO();
    // 创建帧资源
    void BuildFrameResources();
    //创建材质
    void BuildMaterials();
    // 创建渲染对象
    void BuildRenderItems();
    // 绘制渲染对象
    void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, const std::vector<RenderItem*>& ritems);


    float GetHillsHeight(float x, float z)const;
    XMFLOAT3 GetHillsNormal(float x, float z)const;
private:
    std::vector<std::unique_ptr<FrameResource>> mFrameResources;
    FrameResource* mCurrFrameResource = nullptr;
    int mCurrFrameResourceIndex = 0;


    UINT mCbvSrvDescriptorSize = 0;

    // 根签名
    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;
    std::unordered_map<std::string, ComPtr<ID3DBlob>> mShaders;
    std::unordered_map<std::string, ComPtr<ID3D12PipelineState>> mPSOs;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

   /* ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;*/

    RenderItem* mWavesRitem = nullptr;
    // 待渲染的成员
    std::vector<std::unique_ptr<RenderItem>> mAllRitems;

    // Render items divided by PSO.
    std::vector<RenderItem*> mRitemLayer[(int)RenderLayer::Count];

    std::unique_ptr<Waves> mWaves;

    PassConstants mMainPassCB;


    XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
    XMFLOAT4X4 mView = MathHelper::Identity4x4();
    XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    float mTheta = 1.5f * XM_PI;
    float mPhi = XM_PIDIV4;
    float mRadius = 50.0f;

    float mSunTheta = 1.25f * XM_PI;
    float mSunPhi = XM_PIDIV4;

    POINT mLastMousePos;


};

#endif