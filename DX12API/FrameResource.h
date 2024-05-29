#pragma once#pragma once

#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"
//帧资源处理
struct ObjectConstants
{
	DirectX::XMFLOAT4X4 World = MathHelper::Identity4x4();
};
//渲染过程中所需的常量数据
struct PassConstants
{
	DirectX::XMFLOAT4X4 View = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvView = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 Proj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 ViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT4X4 InvViewProj = MathHelper::Identity4x4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;
};
struct Vertex
{
	DirectX::XMFLOAT4 Pos;
	DirectX::XMFLOAT4 Color;
};

// Stores the resources needed for the CPU to build the command lists for a frame.  
// 存储CPU为构建每帧命令列表所需的资源
// 其中数据依程序而异，取决于实际绘制所需的资源
struct FrameResource
{
public:
	FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
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
	std::unique_ptr<UploadBuffer<ObjectConstants>> ObjectCB = nullptr;

	// Fence value to mark commands up to this fence point.  This lets us
	// check if these frame resources are still in use by the GPU.
	// 通过围栏值将命令标记到此围栏点，这使我们可以检测到GPU是否还在使用这些帧资源
	UINT64 Fence = 0;

};