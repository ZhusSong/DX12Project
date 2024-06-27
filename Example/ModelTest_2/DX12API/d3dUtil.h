#pragma once
#include <windows.h>
#include <wrl.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
#include <chrono>
#include <iostream>
#include <strsafe.h>
#include "d3dx12.h"
#include "DDSTextureLoader.h"
#include "MathHelper.h"
#include "d3dDebugLogger.h"

#define MAX_OUTPUT_BUFFER_LEN 1024
extern const int gNumFrameResources;
inline void d3dSetDebugName(IDXGIObject* obj, const char* name)
{
    if (obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12Device* obj, const char* name)
{
    if (obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}
inline void d3dSetDebugName(ID3D12DeviceChild* obj, const char* name)
{
    if (obj)
    {
        obj->SetPrivateData(WKPDID_D3DDebugObjectName, lstrlenA(name), name);
    }
}

inline std::wstring AnsiToWString(const std::string& str)
{
    WCHAR buffer[512];
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
    return std::wstring(buffer);
}
//
//__inline void PrintfW(LPCWSTR pszFormat, ...)
//{
//    WCHAR pBuffer[MAX_OUTPUT_BUFFER_LEN] = {};
//    size_t szStrLen = 0;
//    va_list va;
//    va_start(va, pszFormat);
//    if (S_OK != ::StringCchVPrintfW(pBuffer, _countof(pBuffer), pszFormat, va))
//    {
//        va_end(va);
//        return;
//    }
//    va_end(va);
//
//    StringCchLengthW(pBuffer, MAX_OUTPUT_BUFFER_LEN, &szStrLen);
//
//    (szStrLen > 0) ?
//        WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), pBuffer, (DWORD)szStrLen, nullptr, nullptr)
//        : 0;
//}
//__inline void PrintfA(LPCSTR pszFormat, ...)
//{
//    CHAR pBuffer[MAX_OUTPUT_BUFFER_LEN] = {};
//    size_t szStrLen = 0;
//
//    va_list va;
//    va_start(va, pszFormat);
//    if (S_OK != ::StringCchVPrintfA(pBuffer, _countof(pBuffer), pszFormat, va))
//    {
//        va_end(va);
//        return;
//    }
//    va_end(va);
//
//    StringCchLengthA(pBuffer, MAX_OUTPUT_BUFFER_LEN, &szStrLen);
//
//    (szStrLen > 0) ?
//        WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), pBuffer, (DWORD)szStrLen, nullptr, nullptr)
//        : 0;
//}
/*
#if defined(_DEBUG)
    #ifndef Assert
    #define Assert(x, description)                                  \
    {                                                               \
        static bool ignoreAssert = false;                           \
        if(!ignoreAssert && !(x))                                   \
        {                                                           \
            Debug::AssertResult result = Debug::ShowAssertDialog(   \
            (L#x), description, AnsiToWString(__FILE__), __LINE__); \
        if(result == Debug::AssertIgnore)                           \
        {                                                           \
            ignoreAssert = true;                                    \
        }                                                           \
                    else if(result == Debug::AssertBreak)           \
        {                                                           \
            __debugbreak();                                         \
        }                                                           \
        }                                                           \
    }
    #endif
#else
    #ifndef Assert
    #define Assert(x, description)
    #endif
#endif
    */

class d3dUtil
{
public:

    static bool IsKeyDown(int vkeyCode);

    static std::string ToString(HRESULT hr);

    static UINT CalcConstantBufferByteSize(UINT byteSize)
    {
        // Constant buffers must be a multiple of the minimum hardware
        // allocation size (usually 256 bytes).  So round up to nearest
        // multiple of 256.  We do this by adding 255 and then masking off
        // the lower 2 bytes which store all bits < 256.
        // Example: Suppose byteSize = 300.
        // (300 + 255) & ~255
        // 555 & ~255
        // 0x022B & ~0x00ff
        // 0x022B & 0xff00
        // 0x0200
        // 512
        return (byteSize + 255) & ~255;
    }

    static Microsoft::WRL::ComPtr<ID3DBlob> LoadBinary(const std::wstring& filename);

    static Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
        ID3D12Device* device,
        ID3D12GraphicsCommandList* cmdList,
        const void* initData,
        UINT64 byteSize,
        Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

    static Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
        const std::wstring& filename,
        const D3D_SHADER_MACRO* defines,
        const std::string& entrypoint,
        const std::string& target);
};

class DxException
{
public:
    DxException() = default;
    DxException(HRESULT hr, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

    std::wstring ToString()const;

    HRESULT ErrorCode = S_OK;
    std::wstring FunctionName;
    std::wstring Filename;
    int LineNumber = -1;
};

// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
// geometries are stored in one vertex and index buffer.  It provides the offsets
// and data needed to draw a subset of geometry stores in the vertex and index 
// buffers so that we can implement the technique described by Figure 6.3.
struct SubmeshGeometry
{
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    INT BaseVertexLocation = 0;

    // Bounding box of the geometry defined by this submesh. 
    // This is used in later chapters of the book.
    DirectX::BoundingBox Bounds;
};

struct MeshGeometry
{
    // Give it a name so we can look it up by name.
    std::string Name;

    // System memory copies.  Use Blobs because the vertex/index format can be generic.
    // It is up to the client to cast appropriately.  
    Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

    // Data about the buffers.
    UINT VertexBufferOffset = 0;
    UINT VertexBufferByteSize = 0;
    UINT VertexByteStride = 0;

    UINT IndexBufferOffset = 0;
    UINT IndexBufferByteSize = 0;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;

    // A MeshGeometry may store multiple geometries in one vertex/index buffer.
    // Use this container to define the Submesh geometries so we can draw
    // the Submeshes individually.
    std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView()const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView()const
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = IndexFormat;
        ibv.SizeInBytes = IndexBufferByteSize;

        return ibv;
    }

    // We can free this memory after we finish upload to the GPU.
    void DisposeUploaders()
    {
        VertexBufferUploader = nullptr;
        IndexBufferUploader = nullptr;
    }
};

struct Light
{
    // 光照的颜色
    DirectX::XMFLOAT3 Strength = { 0.5f, 0.5f, 0.5f };
    // point/spot light only
    // 仅供点光/聚光灯使用
    float FalloffStart = 1.0f;
    // directional/spot light only
    // 仅供方向光/聚光灯使用
    DirectX::XMFLOAT3 Direction = { 0.0f, -1.0f, 0.0f };
    // point/spot light only
    // 仅供点光/聚光灯使用
    float FalloffEnd = 10.0f;
    // point/spot light only
    // 仅供点光/聚光灯使用
    DirectX::XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
    // spot light only
    // 仅供聚光灯使用
    float SpotPower = 64.0f;
};

#define MaxLights 16

struct MaterialConstants
{
    DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
    float Roughness = 0.25f;

    // Used in texture mapping.
    DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

// Simple struct to represent a material for our demos.  A production 3D engine
// would likely create a class hierarchy of Materials.
// 表示材质的简单结构体
struct Material
{
    // Unique material name for lookup.
    // 便于查找材质的唯一对应名称
    std::string Name;

    // Index into constant buffer corresponding to this material.
    // 本材质的常量缓冲区索引
    int MatCBIndex = -1;

    // Index into SRV heap for diffuse texture.
    // 漫反射纹理在SRV堆中的索引
    int DiffuseSrvHeapIndex = -1;

    // Index into SRV heap for normal texture.
    // 
    int NormalSrvHeapIndex = -1;

    // Dirty flag indicating the material has changed and we need to update the constant buffer.
    // Because we have a material constant buffer for each FrameResource, we have to apply the
    // update to each FrameResource.  Thus, when we modify a material we should set 
    // NumFramesDirty = gNumFrameResources so that each frame resource gets the update.
    // 已更新标志(DirtyFlag)，表示本材质已有变动，而我们也需要更新常量缓冲区
    // 由于每个帧资源都有一个材质常量缓冲区，所以必须对每个帧资源都进行更新
    // 因此当修改某个材质时，应该设置NumFrameDirty=gNumFrameResources使每个帧资源都能得到更新
    int NumFramesDirty = gNumFrameResources;

    // Material constant buffer data used for shading.
    // 用于着色的材质常量缓冲区数据
    // 漫反射反照率
    DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
    // 材质属性
    DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
    // 粗糙度
    float Roughness = .25f;

    DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};

struct Texture
{
    // Unique material name for lookup.
    std::string Name;

    std::wstring Filename;

    Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
};

#ifndef ThrowIfFailed
#define ThrowIfFailed(x)                                              \
{                                                                     \
    HRESULT hr__ = (x);                                               \
    std::wstring wfn = AnsiToWString(__FILE__);                       \
    if(FAILED(hr__)) { throw DxException(hr__, L#x, wfn, __LINE__); } \
}
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif
