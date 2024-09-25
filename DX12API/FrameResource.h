#pragma once

#include "d3dUtil.h"
#include "MathHelper.h"
#include "UploadBuffer.h"


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