#include "RenderState.h"
#include "d3dUtil.h"
#include <cstring>      

using namespace Microsoft::WRL;

// 初始化
ComPtr<D3D12_RASTERIZER_DESC> RenderState::RSWireframe = nullptr;
ComPtr<D3D12_RASTERIZER_DESC> RenderState::RSNoCull = nullptr;
ComPtr<D3D12_RASTERIZER_DESC> RenderState::RSCullClockWise = nullptr;

ComPtr<D3D12_SAMPLER_DESC> RenderState::SSLinearWrap = nullptr;
ComPtr<D3D12_SAMPLER_DESC> RenderState::SSAnisotropicWrap = nullptr;

ComPtr<D3D12_BLEND_DESC> RenderState::BSAlphaToCoverage = nullptr;
ComPtr<D3D12_BLEND_DESC> RenderState::BSNoColorWrite = nullptr;
ComPtr<D3D12_BLEND_DESC> RenderState::BSTransparent = nullptr;
ComPtr<D3D12_BLEND_DESC> RenderState::BSAdditive = nullptr;

ComPtr<D3D12_DEPTH_STENCIL_DESC> RenderState::DSSWriteStencil = nullptr;
ComPtr<D3D12_DEPTH_STENCIL_DESC> RenderState::DSSDrawWithStencil = nullptr;
ComPtr<D3D12_DEPTH_STENCIL_DESC> RenderState::DSSNoDoubleBlend = nullptr;
ComPtr<D3D12_DEPTH_STENCIL_DESC> RenderState::DSSNoDepthTest = nullptr;
ComPtr<D3D12_DEPTH_STENCIL_DESC> RenderState::DSSNoDepthWrite = nullptr;
ComPtr<D3D12_DEPTH_STENCIL_DESC> RenderState::DSSNoDepthTestWithStencil = nullptr;
ComPtr<D3D12_DEPTH_STENCIL_DESC> RenderState::DSSNoDepthWriteWithStencil = nullptr;

bool RenderState::IsInit()
{
	// 判断是否已进行过初始化
	return RSWireframe != nullptr;
}

void RenderState::InitAll(ID3D12Device* device)
{
	// 通过第一个进行定义的状态:RSWireframe 是否为空判断是否已进行过初始化
	if (IsInit())
		return;

	
	// 初始化光栅化状态
	D3D12_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));

	// 线框模式
	rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.DepthClipEnable = true;
}
