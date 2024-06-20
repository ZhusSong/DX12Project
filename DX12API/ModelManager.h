#pragma once

#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H
// 模型读取相关
#include <d3d11_1.h>
#include <wrl/client.h>
#include <filesystem>
#include "d3dUtil.h"
#include <DirectXMath.h>


#define ASSIMP_LOAD_FLAGS 0
class ModelManager
{
public:

	ModelManager() = default;
	~ModelManager() = default;

	ModelManager(const ModelManager&) = default;
	ModelManager& operator=(const ModelManager&) = default;

	ModelManager(ModelManager&&) = default;
	ModelManager& operator=(ModelManager&&) = default;

private:
	void LoadModels();


};
#endif