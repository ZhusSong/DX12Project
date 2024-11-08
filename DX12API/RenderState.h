#pragma once

#ifndef RENDERSTATES_H
#define RENDERSTATES_H

#include "WinAPISetting.h"
#include <wrl/client.h>
#include <d3d12.h>

// 提供一些已定义好的PSO状态
// Provides some defined PSO states
class RenderState
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;


    static bool IsInit();

    static void InitAll(ID3D12Device* device);

public:

    // 预设置一些渲染状态
	// 光栅化状态:线框
    static ComPtr<D3D12_RASTERIZER_DESC> RSWireframe;

    // 光栅化状态:无背面裁剪
    static ComPtr<D3D12_RASTERIZER_DESC> RSNoCull;

    // 光栅化状态:顺时针裁剪
    static ComPtr<D3D12_RASTERIZER_DESC> RSCullClockWise;

    //采样器状态:线性过滤
    static ComPtr<D3D12_SAMPLER_DESC> SSLinearWrap;

    //采样器状态:各向异性过滤
    static ComPtr<D3D12_SAMPLER_DESC> SSAnisotropicWrap;

    //混合状态:不写入颜色
    static ComPtr<D3D12_BLEND_DESC> BSNoColorWrite;

    //混合状态:透明混合
    static ComPtr<D3D12_BLEND_DESC> BSTransparent;

    //混合状态:Alpha-To-Coverage
    static ComPtr<D3D12_BLEND_DESC> BSAlphaToCoverage;

    //混合状态:加法混合
    static ComPtr<D3D12_BLEND_DESC> BSAdditive;

    //深度模板状态:写入模板值
    static ComPtr<D3D12_DEPTH_STENCIL_DESC> DSSWriteStencil;

    //深度模板状态:对指定模板值的区域进行绘制
    static ComPtr<D3D12_DEPTH_STENCIL_DESC> DSSDrawWithStencil;

    //深度模板状态:无二次混合区域
    static ComPtr<D3D12_DEPTH_STENCIL_DESC> DSSNoDoubleBlend;

    //深度模板状态:关闭深度测试
    static ComPtr<D3D12_DEPTH_STENCIL_DESC> DSSNoDepthTest;

    //深度模板状态:仅进行深度测试，不写入深度值
    static ComPtr<D3D12_DEPTH_STENCIL_DESC> DSSNoDepthWrite;

    //深度模板状态:关闭深度测试，对指定模板值的区域进行绘制
    static ComPtr<D3D12_DEPTH_STENCIL_DESC> DSSNoDepthTestWithStencil;

    //深度模板状态:仅进行深度测试，不写入深度值，对指定模板值的区域进行绘制
    static ComPtr<D3D12_DEPTH_STENCIL_DESC> DSSNoDepthWriteWithStencil;
};

#endif