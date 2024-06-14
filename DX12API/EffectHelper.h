#pragma once

//定义了一些实用的渲染模型
//该头文件需要再包含特效类的源文件中使用，且需在Effects.h与DX11Utility.h后声明

#ifndef EFFECTHELPER_H
#define EFFECTHELPER_H

//若需要进行内存对齐，从该类派生
template<class DerivedType>
struct AlignedType
{
	static void* operator new(size_t size)
	{
		const size_t alignedSize = __alignof(DerivedType);

		static_assert(alignedSize > 8, "AlignedNew is only useful for types with > 8 byte alignment! Did you forget a __declspec(align) on DerivedType?");

		void* ptr = _aligned_malloc(size, alignedSize);

		if (!ptr)
			throw std::bad_alloc();

		return ptr;
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



};

#endif