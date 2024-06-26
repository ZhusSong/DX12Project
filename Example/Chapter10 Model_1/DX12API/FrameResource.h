#pragma once

#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
//帧资源处理
struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();
};
//渲染过程中所需的常量数据
struct PassConstants
{
	//视图矩阵，代表摄像机在世界空间中的位置与方向
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	//视图矩阵的逆矩阵，用于将坐标从屏幕空间转回世界空间
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	//投影矩阵，用于将3D坐标转换为2D坐标用于渲染
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	//投影矩阵的逆，用于将坐标从屏幕空间转回世界空间
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	//视图与投影矩阵的结合，为其相乘的结果
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	//上述矩阵的逆，用于将坐标从屏幕空间转回世界空间
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	//摄像机在世界空间中的位置
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	//用于对齐常量缓冲区的填充值
	float cbPerObjectPad1 = 0.0f;
	//渲染目标的尺寸
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	//渲染目标尺寸的倒数，用于UV计算或缩放
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	//近裁切面距离，与摄像机距离小于此值的物体不会被渲染
	float NearZ = 0.0f;
	//远裁切面距离，与摄像机距离大于此值的物体不会被渲染
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	DirectX::XMFLOAT4 FogColor = { 0.3f, 0.3f, 0.3f, 1.0f };
	// 雾气的起始点与范围
	float gFogStart = 5.0f;
	float gFogRange = 150.0f;
	DirectX::XMFLOAT2 cbPerObjectPad2;

	Light Lights[MaxLights];
};

struct Vertex
{
	Vertex() = default;
	Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v) :
		Pos(x, y, z),
		Normal(nx, ny, nz),
		TexC(u, v) {}

	DirectX::XMFLOAT3 Pos;
	DirectX::XMFLOAT3 Normal;
	DirectX::XMFLOAT2 TexC;
};

// Stores the resources needed for the CPU to build the command lists for a frame.  
// 存储CPU为构建每帧命令列表所需的资源
// 其中数据依程序而异，取决于实际绘制所需的资源
struct FrameResource
{
public:
	// 若无波浪绘制则waveVertCount= -1 
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount);
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount, UINT waveVertCount);
	FrameResource(const FrameResource& rhs) = delete;
	FrameResource& operator=(const FrameResource& rhs) = delete;
	~FrameResource();
    // We cannot reset the allocator until the GPU is done processing the commands.
    // So each frame needs their own allocator.
	// 在GPU处理完与此命令分配器相关的命令之前，我们不能对其进行重置
	// 因此每一帧都需要它们自己的命令分配器
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> CmdListAlloc;

	// We cannot update a cbuffer until the GPU is done processing the commands
	// that reference it.  So each frame needs their own cbuffers.
	// 在GPU执行完引用此常量缓冲区的命令之前，我们不能对其进行重置
	// 因此每一帧都需要它们自己的常量缓冲区
	std::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> MaterialCB = nullptr;
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;


	// We cannot update a dynamic vertex buffer until the GPU is done processing
	// the commands that reference it.  So each frame needs their own.
	std::unique_ptr<UploadBuffer<Vertex>> WavesVB = nullptr;

	// Fence value to mark commands up to this fence point.  This lets us
	// check if these frame resources are still in use by the GPU.
	// 通过围栏值将命令标记到此围栏点，这使我们可以检测到GPU是否还在使用这些帧资源
	UINT64 Fence = 0;

};