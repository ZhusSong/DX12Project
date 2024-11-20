#pragma once

//游戏对象类，集成了一个对象所需的组件
#ifndef  GAMEOBJECT_H
#define GAMEOBJECT_H

#include "GeometryGenerator.h"
#include "Transform.h"
#include "RenderState.h"
#include "d3dUtil.h"

class GameObject
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	GameObject();

	Transform& GetTransform();

	// 获取物体变换
	const Transform& GetTransform() const;

	// 设置描述
	void SetBuffer();

	// 设置纹理
	void SetTexture();

	// 设置材质
	void SetMaterial();

	// 绘制
	void Draw();

private:
	// 物体变换信息
	Transform mTransform;

	// 材质信息
	Material mMaterial;

	// 物体贴图
	ComPtr<ID3D12Resource> mTexture;
	// 定点缓冲区
	ComPtr<ID3D12Resource> mVertexBuffer;
	// 索引缓冲区
	ComPtr<ID3D12Resource> mIndexBuffer;
	// 顶点字节大小
	UINT mVertexStride;
	// 索引数目
	UINT mIndexCount;

};

#endif