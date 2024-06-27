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

	// 判断是否已进行过初始化
	static bool IsInit();
	// 初始化所有PSO状态
	static void InitAll(ID3D12Device* device);

public:

};

#endif