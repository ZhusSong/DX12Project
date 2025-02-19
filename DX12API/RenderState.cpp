#include "RenderState.h"
#include "d3dUtil.h"
#include <cstring>      

using namespace Microsoft::WRL;
// 初始化静态成员变量
D3D12_RASTERIZER_DESC RenderState::RSWireframe = {};
D3D12_RASTERIZER_DESC RenderState::RSNoCull = {};
D3D12_RASTERIZER_DESC RenderState::RSCullClockWise = {};

D3D12_SAMPLER_DESC RenderState::SSLinearWrap = {};
D3D12_SAMPLER_DESC RenderState::SSAnisotropicWrap = {};

D3D12_BLEND_DESC RenderState::BSNoColorWrite = {};
D3D12_BLEND_DESC RenderState::BSTransparent = {};
D3D12_BLEND_DESC RenderState::BSAlphaToCoverage = {};
D3D12_BLEND_DESC RenderState::BSAdditive = {};

D3D12_DEPTH_STENCIL_DESC RenderState::DSSWriteStencil = {};
D3D12_DEPTH_STENCIL_DESC RenderState::DSSDrawWithStencil = {};
D3D12_DEPTH_STENCIL_DESC RenderState::DSSNoDoubleBlend = {};
D3D12_DEPTH_STENCIL_DESC RenderState::DSSNoDepthTest = {};
D3D12_DEPTH_STENCIL_DESC RenderState::DSSNoDepthWrite = {};
D3D12_DEPTH_STENCIL_DESC RenderState::DSSNoDepthTestWithStencil = {};
D3D12_DEPTH_STENCIL_DESC RenderState::DSSNoDepthWriteWithStencil = {};

bool RenderState::IsInit()
{
    // 判断是否已进行过初始化
    return RSWireframe.FillMode != D3D12_FILL_MODE_WIREFRAME;
}

void RenderState::InitAll(ID3D12Device* device)
{
    if (IsInit())
        return;

    // 初始化光栅化状态
    //

    // 线框模式
    RSWireframe.FillMode = D3D12_FILL_MODE_WIREFRAME;
    RSWireframe.CullMode = D3D12_CULL_MODE_NONE;
    RSWireframe.FrontCounterClockwise = FALSE;
    RSWireframe.DepthClipEnable = TRUE;

    // 无背面剔除模式
    RSNoCull.FillMode = D3D12_FILL_MODE_SOLID;
    RSNoCull.CullMode = D3D12_CULL_MODE_NONE;
    RSNoCull.FrontCounterClockwise = FALSE;
    RSNoCull.DepthClipEnable = TRUE;

    // 顺时针剔除模式
    RSCullClockWise.FillMode = D3D12_FILL_MODE_SOLID;
    RSCullClockWise.CullMode = D3D12_CULL_MODE_BACK;
    RSCullClockWise.FrontCounterClockwise = TRUE;
    RSCullClockWise.DepthClipEnable = TRUE;

    // 初始化采样器状态
    //

    // 线性过滤模式
    SSLinearWrap.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    SSLinearWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    SSLinearWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    SSLinearWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    SSLinearWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    SSLinearWrap.MinLOD = 0;
    SSLinearWrap.MaxLOD = D3D12_FLOAT32_MAX;

    // 各向异性过滤模式
    SSAnisotropicWrap.Filter = D3D12_FILTER_ANISOTROPIC;
    SSAnisotropicWrap.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    SSAnisotropicWrap.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    SSAnisotropicWrap.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
    SSAnisotropicWrap.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    SSAnisotropicWrap.MaxAnisotropy = 16;
    SSAnisotropicWrap.MinLOD = 0;
    SSAnisotropicWrap.MaxLOD = D3D12_FLOAT32_MAX;

    // 初始化混合状态
    //

    // Alpha-To-Coverage 模式
    BSAlphaToCoverage.AlphaToCoverageEnable = TRUE;
    BSAlphaToCoverage.IndependentBlendEnable = FALSE;
    BSAlphaToCoverage.RenderTarget[0].BlendEnable = FALSE;
    BSAlphaToCoverage.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    // 透明混合模式
    // Color = SrcAlpha * SrcColor + (1 - SrcAlpha) * DestColor 
    // Alpha = SrcAlpha
    BSTransparent.AlphaToCoverageEnable = FALSE;
    BSTransparent.IndependentBlendEnable = FALSE;
    BSTransparent.RenderTarget[0].BlendEnable = TRUE;
    BSTransparent.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
    BSTransparent.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    BSTransparent.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
    BSTransparent.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
    BSTransparent.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
    BSTransparent.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;

    // 加法混合模式
    // Color = SrcColor + DestColor
    // Alpha = SrcAlpha
    BSAdditive = BSTransparent;
    BSAdditive.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
    BSAdditive.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

    // 无颜色写入混合模式
    // Color = DestColor
    // Alpha = DestAlphas
    BSNoColorWrite = BSTransparent;
    BSNoColorWrite.RenderTarget[0].BlendEnable = FALSE;
    BSNoColorWrite.RenderTarget[0].RenderTargetWriteMask = 0;

    // 初始化深度模板状态
    //

    // 写入模板值的深度模板状态
    // 不写入深度信息
    // 无论是正面还是背面，原来指定的区域的模板值都会被写入StencilRef
    DSSWriteStencil.DepthEnable = TRUE;
    DSSWriteStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    DSSWriteStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    DSSWriteStencil.StencilEnable = TRUE;
    DSSWriteStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    DSSWriteStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    DSSWriteStencil.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    DSSWriteStencil.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DSSWriteStencil.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
    DSSWriteStencil.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    // 对于背面的几何体我们不进行渲染，所以此处设置为与正面相同
    DSSWriteStencil.BackFace = DSSWriteStencil.FrontFace;

    // 对指定模板值进行绘制的深度/模板状态
    // 对满足模板值条件的区域才进行绘制，并更新深度
    DSSDrawWithStencil.DepthEnable = TRUE;
    DSSDrawWithStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    DSSDrawWithStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    DSSDrawWithStencil.StencilEnable = TRUE;
    DSSDrawWithStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    DSSDrawWithStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    DSSDrawWithStencil.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    DSSDrawWithStencil.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DSSDrawWithStencil.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    DSSDrawWithStencil.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

    // 对于背面的几何体我们不进行渲染，所以此处设置为与正面相同
    DSSDrawWithStencil.BackFace = DSSDrawWithStencil.FrontFace;

    // 无二次混合深度/模板状态
    // 允许默认深度测试
    // 通过自递增使得原来StencilRef的值只能使用一次，实现仅一次混合
    DSSNoDoubleBlend.DepthEnable = TRUE;
    DSSNoDoubleBlend.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    DSSNoDoubleBlend.DepthFunc = D3D12_COMPARISON_FUNC_LESS;

    DSSNoDoubleBlend.StencilEnable = TRUE;
    DSSNoDoubleBlend.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    DSSNoDoubleBlend.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    DSSNoDoubleBlend.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDoubleBlend.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDoubleBlend.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
    DSSNoDoubleBlend.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

    // 对于背面的几何体我们不进行渲染，所以此处设置为与正面相同
    DSSNoDoubleBlend.BackFace = DSSNoDoubleBlend.FrontFace;

    // 关闭深度测试的深度/模板状态
    // 若绘制非透明物体，务必严格按照绘制顺序
    // 绘制透明物体则不需要担心绘制顺序
    // 而默认情况下模板测试就是关闭的
    DSSNoDepthTest.DepthEnable = false;
    DSSNoDepthTest.StencilEnable = false;

    // 关闭深度测试，只进行模板测试
    // 若绘制非透明物体，务必严格按照绘制顺序
    // 绘制透明物体则不需要担心绘制顺序
    // 对满足模板值条件的区域才进行绘制
    DSSNoDepthTestWithStencil.StencilEnable = TRUE;
    DSSNoDepthTestWithStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    DSSNoDepthTestWithStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    DSSNoDepthTestWithStencil.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDepthTestWithStencil.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDepthTestWithStencil.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDepthTestWithStencil.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

    // 对于背面的几何体我们不进行渲染，所以此处设置为与正面相同
    DSSNoDepthTestWithStencil.BackFace = DSSDrawWithStencil.FrontFace;

    // 进行深度测试，但不写入深度值的状态
    // 若绘制非透明物体时，应使用默认状态
    // 绘制透明物体时，使用该状态可以有效确保混合状态的进行
    // 并且确保较前的非透明物体可以阻挡较后的一切物体
    DSSNoDepthWrite.DepthEnable = TRUE;
    DSSNoDepthWrite.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    DSSNoDepthWrite.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    DSSNoDepthWrite.StencilEnable = FALSE;

    // 进行深度测试，但不写入深度值的状态，同时进行模板测试
    // 若绘制非透明物体时，应使用默认状态
    // 绘制透明物体时，使用该状态可以有效确保混合状态的进行
    // 并且确保较前的非透明物体可以阻挡较后的一切物体
    // 对满足模板值条件的区域才进行绘制
    DSSNoDepthWriteWithStencil.StencilEnable = true;
    DSSNoDepthWriteWithStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    DSSNoDepthWriteWithStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;

    DSSNoDepthWriteWithStencil.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDepthWriteWithStencil.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDepthWriteWithStencil.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    DSSNoDepthWriteWithStencil.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
    // 对于背面的几何体我们是不进行渲染的，所以这里的设置无关紧要
    DSSNoDepthWriteWithStencil.BackFace = DSSNoDepthWriteWithStencil.FrontFace;
   
  
}
