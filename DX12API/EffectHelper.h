#pragma once
//定义了一些实用的渲染模型
//该头文件需要再包含特效类的源文件中使用，且需在Effects.h与DX11Utility.h后声明

#ifndef EFFECTHELPER_H
#define EFFECTHELPER_H

#include <d3d12.h>
#include <wrl/client.h>
#include <memory>
#include <stdexcept>

//若需要进行内存对齐，从该类派生
template<class DerivedType>
struct AlignedType
{
	static void* operator new(size_t size)
	{
		static void* operator new(size_t size)
		{
			constexpr size_t alignment = std::max(alignof(DerivedType), D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT);
			void* ptr = _aligned_malloc(size, alignment);
			if (!ptr) 
				throw std::bad_alloc();
			return ptr;
		}
	}
	static void operator delete(void* ptr)
	{
		_aligned_free(ptr);
	}
};

//抽象类，仅做为接口提供模板描述
//常量缓冲区管理
struct CBufferBase
{
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	virtual ~CBufferBase() = default;
	virtual HRESULT CreateBuffer(ID3D12Device* device) = 0;
	virtual void UpdateBuffer() = 0;
	virtual void BindVS(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void BindPS(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void BindHS(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void BindDS(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void BindGS(ID3D12GraphicsCommandList* cmdList) = 0;
	virtual void BindCS(ID3D12GraphicsCommandList* cmdList) = 0;

protected:
	ComPtr<ID3D12Resource> m_resource;
	void* m_mappedData = nullptr;
	bool m_dirty = true;
};

template<UINT RootParamIndex, typename T>
struct CBufferObject : CBufferBase
{
    T data;

    HRESULT CreateBuffer(ID3D12Device* device) override
    {
        if (m_resource) return S_OK;

        const UINT bufferSize = (sizeof(T) + 255) & ~255;
        const auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        const auto resDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_resource));

        if (SUCCEEDED(hr)) {
            CD3DX12_RANGE readRange(0, 0);
            m_resource->Map(0, &readRange, &m_mappedData);
        }
        return hr;
    }

    void UpdateBuffer() override
    {
        if (m_dirty && m_mappedData) {
            memcpy(m_mappedData, &data, sizeof(T));
            m_dirty = false;
        }
    }

    void BindVS(ID3D12GraphicsCommandList* cmdList) override
    {
        cmdList->SetGraphicsRootConstantBufferView(
            RootParamIndex,
            m_resource->GetGPUVirtualAddress()
        );
    }

    void BindPS(ID3D12GraphicsCommandList* cmdList) override
    {
        cmdList->SetGraphicsRootConstantBufferView(
            RootParamIndex,
            m_resource->GetGPUVirtualAddress()
        );
    }

};
#endif