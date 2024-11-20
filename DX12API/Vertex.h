#pragma once

//***************************************************************************************
// 定义了一些顶点结构体和输入布局
// Defines vertex structures and input layouts.
//***************************************************************************************

#ifndef  VERTEX_H
#define VERTEX_H
#include<d3d12.h>
#include<DirectXMath.h>

/// <summary>
/// 仅有顶点位置信息
/// </summary>
struct VertexPos
{
    VertexPos() = default;

    VertexPos(const VertexPos&) = default;
    VertexPos& operator=(const VertexPos&) = default;

    VertexPos(VertexPos&&) = default;
    VertexPos& operator=(VertexPos&&) = default;

    constexpr VertexPos(const DirectX::XMFLOAT3& _pos) : pos(_pos) {}

    DirectX::XMFLOAT3 pos;
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[1];
};

/// <summary>
/// 有顶点位置与颜色信息
/// </summary>
struct VertexPosColor
{
    VertexPosColor() = default;

    VertexPosColor(const VertexPosColor&) = default;
    VertexPosColor& operator=(const VertexPosColor&) = default;

    VertexPosColor(VertexPosColor&&) = default;
    VertexPosColor& operator=(VertexPosColor&&) = default;

    constexpr VertexPosColor(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT4& _color) :
        pos(_pos), color(_color) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[2];
};

/// <summary>
/// 有贴图信息
/// </summary>
struct VertexPosTex
{
    VertexPosTex() = default;

    VertexPosTex(const VertexPosTex&) = default;
    VertexPosTex& operator=(const VertexPosTex&) = default;

    VertexPosTex(VertexPosTex&&) = default;
    VertexPosTex& operator=(VertexPosTex&&) = default;

    constexpr VertexPosTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT2& _tex) :
        pos(_pos), tex(_tex) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 tex;
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[2];
};

/// <summary>
/// 
/// </summary>
struct VertexPosSize
{
    VertexPosSize() = default;

    VertexPosSize(const VertexPosSize&) = default;
    VertexPosSize& operator=(const VertexPosSize&) = default;

    VertexPosSize(VertexPosSize&&) = default;
    VertexPosSize& operator=(VertexPosSize&&) = default;

    constexpr VertexPosSize(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT2& _size) :
        pos(_pos), size(_size) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 size;
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[2];
};

struct VertexPosNormalColor
{
    VertexPosNormalColor() = default;

    VertexPosNormalColor(const VertexPosNormalColor&) = default;
    VertexPosNormalColor& operator=(const VertexPosNormalColor&) = default;

    VertexPosNormalColor(VertexPosNormalColor&&) = default;
    VertexPosNormalColor& operator=(VertexPosNormalColor&&) = default;

    constexpr VertexPosNormalColor(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
        const DirectX::XMFLOAT4& _color) :
        pos(_pos), normal(_normal), color(_color) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 color;
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[3];
};


struct VertexPosNormalTex
{
    VertexPosNormalTex() = default;

    VertexPosNormalTex(const VertexPosNormalTex&) = default;
    VertexPosNormalTex& operator=(const VertexPosNormalTex&) = default;

    VertexPosNormalTex(VertexPosNormalTex&&) = default;
    VertexPosNormalTex& operator=(VertexPosNormalTex&&) = default;

    constexpr VertexPosNormalTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
        const DirectX::XMFLOAT2& _tex) :
        pos(_pos), normal(_normal), tex(_tex) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT2 tex;
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[3];
};

struct VertexPosNormalTangentTex
{
    VertexPosNormalTangentTex() = default;

    VertexPosNormalTangentTex(const VertexPosNormalTangentTex&) = default;
    VertexPosNormalTangentTex& operator=(const VertexPosNormalTangentTex&) = default;

    VertexPosNormalTangentTex(VertexPosNormalTangentTex&&) = default;
    VertexPosNormalTangentTex& operator=(VertexPosNormalTangentTex&&) = default;

    constexpr VertexPosNormalTangentTex(const DirectX::XMFLOAT3& _pos, const DirectX::XMFLOAT3& _normal,
        const DirectX::XMFLOAT4& _tangent, const DirectX::XMFLOAT2& _tex) :
        pos(_pos), normal(_normal), tangent(_tangent), tex(_tex) {}

    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 tangent;
    DirectX::XMFLOAT2 tex;
    static const D3D12_INPUT_ELEMENT_DESC inputLayout[4];
};


#endif
