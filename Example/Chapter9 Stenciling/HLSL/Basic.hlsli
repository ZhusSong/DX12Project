
// Default shader, currently supports lighting.
// 默认着色器 目前已支持着色

// Defaults for number of lights.
// 光源数量的默认值
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
// 包含了光照所用的结构体和函数
#include "LightHelper.hlsli"

//定义纹理对象并分配给特定的纹理寄存器
Texture2D gDiffuseMap : register(t0);


SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5); 
// Constant data that varies per frame.
// 每帧都会变化的常量数据
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gTexTransform;
}

// Constant data that varies per material.
// 绘制过程中所用的杂项常量数据
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    float3 gEyePosW;
    float cbPerObjectPad1;
    float2 gRenderTargetSize;
    float2 gInvRenderTargetSize;
    float gNearZ;
    float gFarZ;
    float gTotalTime;
    float gDeltaTime;
    float4 gAmbientLight;

    // Allow application to change fog parameters once per frame.
	// For example, we may only use fog for certain times of day.
    // 允许程序在每一帧都能改变雾效参数
    // 例如，我们可能只在一天中的特定时间才使用雾效
    float4 gFogColor;
    float gFogStart;
    float gFogRange;
    float2 cbPerObjectPad2;
    
    // Indices [0, NUM_DIR_LIGHTS) are directional lights;
    // indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
    // indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
    // are spot lights for a maximum of MaxLights per object.
    // 对于每个以MaxLights为光源数量最大值的对象来说，索引[0,NUM_DIR_LIGHTS]表示的是方向光源。
    // 索引[NUM_DIR_LIGHTS,NUM_DIR_LIGHTS+NUM_POINT_LIGHTS]表示的是点光源
    // 索引[NUM_DIR_LIGHTS+NUM_POINT_LIGHTS,NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS]表示的是聚光灯光源
    Light gLights[MaxLights];
}
// 每种材质的不同常量数据
cbuffer cbMaterial : register(b2)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMatTransform;
}

struct VertexIn
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC : TEXCOORD;
};
struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
};
